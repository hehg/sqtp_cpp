#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include "sq_frame.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"
#include "plug/sq_plug_obj.h"
#include "sq_mdb.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "sq_struct_def.h"
#include "sq_ts_change.h"
#include "sq_convert.h"
#include "container/shared_queue.h"
#include "pool/object_poll.h"
using namespace sq;
struct trade_msg_data{
		int tid;
		int size;
		int from_id;
		char data[512];
	};
extern void load_md_plugs(const char*);
extern void load_td_plugs(const char*);
static void load_ts(const char*);
extern void unload_td_plugs();
extern void unload_md_plugs();
static void unload_bz_plugs();
static void load_bz_plugs();
static void sq_frame_run();
static std::string m_config_path="../conf/config.xml";
static std::vector<plug_obj*> m_bz_plugs;
static std::mutex s_run_flag_lock;
static bool m_run_flag=false;
static sq_frame_callback_func_t  s_callback_func = nullptr;
static void * s_callback_param=nullptr;

static sq_ts_change s_ts_change;
static  object_pool<trade_msg_data>* m_msg_pool;
static  shared_queue<trade_msg_data> m_trade_msg_queue;
static  std::thread* m_thread=nullptr;
//当前时间
static  uint64_t	s_cur_time_int = 0;
static  sq::trade_time_t  s_cur_time_str;

//插件回调数据
void plugs_callback_func(uint16_t tid, char*data, uint16_t size, void*param, int plug_id)
{
	//todo 这里需要加锁
	if(!m_run_flag){
		return;
	}
	
	// if(tid==tid_market_data){
	// 	sq_quot*quot=(sq_quot*)data;
	// 	uint64_t cur_time = sq::datetime_to_int(quot->time.data());
	// 	s_ts_change.goto_time(cur_time);
	// 	s_cur_time_int = cur_time;
	// 	s_cur_time_str = quot->time;
	// }
	
	//发给应用
	trade_msg_data *msg = m_msg_pool->alloc();
	msg->tid = tid;
	msg->size = size;
	msg->from_id=plug_id;
	assert(size<=sizeof(trade_msg_data::data));
	memcpy(msg->data, data, msg->size);
	m_trade_msg_queue.push(msg);
}
//todo 这个要用一个服务或插件来实现
static void trade_state_changed(const char* name,int type)
{
   //构造一个状态切换消息
	trade_state_changed_ntf pkg;
	pkg.type=type;
	if (strcmp("open", name) == 0)
	{
		pkg.state_id = trade_state_open;
	}
	else if(strcmp("close",name)==0)
	{
		pkg.state_id = trade_state_close;
	}
	else if(strcmp("pause",name)==0)
	{
		pkg.state_id = trade_state_pause;
	}
	else if(strcmp("am_auction",name)==0)
	{
		pkg.state_id = trade_state_am_auction;
	}
	else if(strcmp("pm_auction",name)==0)
	{
		pkg.state_id = trade_state_pm_auction;
	}
	else{
		log_warn("not support trade_state,name={}\n",name);
		return;
	}
   // 发送给biz 插件
	for (auto &it : m_bz_plugs)
	{
		it->plug_put(tid_trade_state, (char*)&pkg, sizeof(pkg));
	}
	// 发给应用
	#if 1
	trade_msg_data *msg = m_msg_pool->alloc();
	msg->tid = tid_trade_state;
	msg->size = sizeof(pkg);
	memcpy(msg->data, (char *)&pkg, msg->size);
	m_trade_msg_queue.push(msg);
	#endif
}

static void init_frame()
{
	m_msg_pool = new object_pool<trade_msg_data>(1000, 100);

	// 读取xml配置文件
	using namespace rapidxml;
	try
	{
		rapidxml::file<> fdoc(m_config_path.c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(fdoc.data());

		xml_node<> *root = doc.first_node();
		xml_node<> *app = root->first_node("app");
		auto log_level = app->first_node("log_level");
		auto log_name = app->first_node("log_name");

		// init log
		if (log_level)
		{
			to_log_level(log_level->value());
		}
		if (log_name)
		{
			std::string name = log_name->value();
			if (name != "" && name != "stdout")
			{
				string log_path = name + ".log";
				s_log.add_file_target(log_path.c_str());
			}
			else
			{
				s_log.add_stdout_target();
			}
		}
		else
		{
			s_log.add_stdout_target();
		}
	}
	catch (const std::exception &e)
	{
		log_error("open config file fail,file={}\n",m_config_path.c_str());
		std::cerr << e.what() << '\n';
		assert(false);
	}

	m_trade_msg_queue.empty();
}

int sq_frame_open(const char*cfg_path)
{
	if(cfg_path){
		m_config_path=cfg_path;
	}
	//加载配置信息
	init_frame();
	//创建内存数据表
	sq_mdb_open();
	//加载业务插件
	load_bz_plugs();

	load_ts(m_config_path.c_str());
	//加载行情插件
	load_md_plugs(m_config_path.c_str());
	//加载交易插件
	load_td_plugs(m_config_path.c_str());

	// open 所有插件
	{
		auto it = m_bz_plugs.begin();
		for (; it != m_bz_plugs.end(); ++it)
		{
			(*it)->plug_open();
		}
	}
	m_run_flag=true;
	m_thread=new std::thread(sq_frame_run);
	return ok;
}

/**
 * 框架内部的线程，负责从应答队列中取出信息回调给上一层
 * 框架的所有业务处理逻辑都在此函数进行
*/
void sq_frame_run()
{
	while (true)
	{
		s_run_flag_lock.lock();
		bool flag = m_run_flag;
		s_run_flag_lock.unlock();

		if (!flag)
		{
			break;
		}

		trade_msg_data *msg = nullptr;
		if (m_trade_msg_queue.try_and_pop(msg))
		{
			int tid=msg->tid;
			char*data= msg->data;
			int size= msg->size;
			int plug_id=msg->from_id;
			// 传给db
			sq_mdb_put(tid, data, size);

			// 传给业务插件
			for (auto &it : m_bz_plugs)
			{
				if (it->plug_id_ != plug_id)
				{
					it->plug_put(tid, data, size);
				}
			}
			//传给应用
			if(s_callback_func)
			{
				s_callback_func(msg->tid, msg->data, msg->size, s_callback_param);
			}

			m_msg_pool->free(msg);
		}
		else{
			SQ_LOG_FLUSH();
		}
	}
	//线程结束了，卸载各个插件
	unload_bz_plugs();
	unload_td_plugs();
	unload_md_plugs();

	delete m_msg_pool;
}
void unload_bz_plugs()
{
	auto it = m_bz_plugs.begin();
	for (; it != m_bz_plugs.end(); ++it)
	{
		(*it)->plug_close();
		(*it)->plug_destory();
	}
	m_bz_plugs.clear();
}
void sq_frame_close()
{
	s_run_flag_lock.lock();
	m_run_flag=false;
	s_run_flag_lock.unlock();
	if(m_thread){
		m_thread->join();
	}
	delete m_thread;
	m_thread=nullptr;
}

void load_ts(const char*cfg_path)
{
	using namespace rapidxml;
	rapidxml::file<> fdoc(cfg_path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *root = doc.first_node();
	xml_node<> *trade_state = root->first_node("trade_state");
    if(trade_state==nullptr){
        return;
    }
    xml_attribute<> *att_enable = trade_state->first_attribute("enable");
    if (att_enable && strcmp(att_enable->value(), "false") == 0)
    {
        return;
    }
	xml_node<> *state = trade_state->first_node("state");

	while (state)
	{
		xml_attribute<> *begin = state->first_attribute("begin");
		xml_attribute<> *end = state->first_attribute("end");
		xml_attribute<> *name = state->first_attribute("name");

		
        s_ts_change.reg_status(begin->value(),
        end->value(),
        name->value());

		state = state->next_sibling();
	}
    s_ts_change.ret_changed_callback(trade_state_changed);
}

void load_bz_plugs()
{
	using namespace rapidxml;
	rapidxml::file<> fdoc(m_config_path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *root = doc.first_node();
	xml_node<> *plugs = root->first_node("plug_bz");
	xml_node<>*plug = plugs->first_node("plug");
	int plug_id = 200;
	while (plug)
	{
		xml_attribute<> *att_enable = plug->first_attribute("enable");
		xml_attribute<> *att_path = plug->first_attribute("path");
		if (att_enable && strcmp(att_enable->value(), "true") == 0)
		{
			plug_obj *obj = new plug_obj();

			int load_ret = obj->load(att_path->value());
			if (load_ret != 0)
			{
				assert(false);
				delete obj;
			}

			obj->plug_create();
			obj->plug_id_ = plug_id;
			++plug_id;
			m_bz_plugs.push_back(obj);
			obj->plug_set_option(PLUG_OPT_id, &obj->plug_id_);
            obj->plug_set_option(PLUG_OPT_callback,(void*)plugs_callback_func);

			xml_node<> *opt = plug->first_node();
			while (opt)
			{
				obj->plug_set_option(opt->name(), opt->value());
				opt = opt->next_sibling();
			}
		}

		plug = plug->next_sibling();
	}
}

void dump_mdb()
{
	sq_mdb_dump();
}

int sq_frame_set_option(const char*key,void*val)
{
	printf("sq_frame_set_option key=%s\n",key);
	if(strcmp(key,"subscribe")==0){
      //format product
	}
	else if(strcmp(key,"callback_func")==0){
		sq_frame_callback_func_t fun=(sq_frame_callback_func_t)val;
		s_callback_func = fun;
	}
	
	else if(strcmp(key,"callback_param")==0)
	{
		s_callback_param=val;
	}
	
	else if (strcmp(key, "config_file") == 0)
	{
		m_config_path = (char*)val;
		printf("set_option,config_path=%s\n",m_config_path.c_str());
	}

	return ok;
}

const char* sq_frame_get_error_msg(int err_no)
{
	return get_err_name(err_no);
}

const char* sq_frame_get_cur_time()
{
	return s_cur_time_str.data();
}
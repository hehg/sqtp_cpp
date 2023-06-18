#include "sq_frame.h"
#include "sq_struct_def.h"
#include "sq_struct_def.h"
#include "sq_mdb.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"
#include "plug/sq_plug_obj.h"
#include "sq_order_table.h"
#include "os/sq_path.h"
using namespace sq;

static sq_order_table* m_order_table=nullptr;

extern void plugs_callback_func(uint16_t tid, char*data, uint16_t size, void*param, int plug_id);
 
static std::vector<plug_obj*> m_td_plugs;

using namespace sq;

void load_td_plugs(const char*cfg_path)
{
	using namespace rapidxml;
	rapidxml::file<> fdoc(cfg_path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *root = doc.first_node();
	xml_node<> *plugs = root->first_node("plug_td");
	xml_node<> *plug_dir = plugs->first_node("plug_dir");
	xml_node<>*plug = plugs->first_node("plug");
	int plug_id = 100;
	while (plug)
	{
		xml_attribute<> *att_enable = plug->first_attribute("enable");
		xml_attribute<> *att_path = plug->first_attribute("path");
		xml_attribute<> *att_name = plug->first_attribute("name");
#if defined (MAC)
        std::string name="lib"+string(att_name->value())+".dylib";
#elif defined(LINUX)
    std::string name="lib"+string(att_name->value())+".so";
#elif defined(WINDOWS)
    std::string name=string(att_name->value())+".dll";
#endif
        string dll_path;
        sq::make_path(dll_path,plug_dir->value(),name.c_str());     
		if (att_enable && strcmp(att_enable->value(), "true") == 0)
		{
			plug_obj *obj = new plug_obj();

			int load_ret = obj->load(dll_path.c_str());
			if (load_ret != 0)
			{
				assert(false);
				delete obj;
			}

			obj->plug_create();
			obj->plug_id_ = plug_id;
			++plug_id;
			m_td_plugs.push_back(obj);

			obj->plug_set_option(PLUG_OPT_id, &obj->plug_id_);
			obj->plug_set_option(PLUG_OPT_callback, (void*)plugs_callback_func);
			xml_node<> *opt = plug->first_node();
			while (opt)
			{
				obj->plug_set_option(opt->name(), opt->value());
				opt = opt->next_sibling();
			}
		}

		plug = plug->next_sibling();
	}

    {
		auto it = m_td_plugs.begin();
		for (; it != m_td_plugs.end(); ++it)
		{
			(*it)->plug_open();
		}
	}
}
void unload_td_plugs()
{
    auto it = m_td_plugs.begin();
		for (; it != m_td_plugs.end(); ++it)
		{
			(*it)->plug_close();
			(*it)->plug_destory();
		}
		m_td_plugs.clear();
}
int place_order(int local_id, const char *market, const char *contract, int direction, int offset, double price,
                int qty, int order_type, int product_type)
{
    assert(market);
    assert(contract);
    log_info("place_order,sq_local_id={},contract={},direction={},offset={},price={},qty={}\n",
    local_id,
    contract,
    direction,
    offset,
    price,
    qty);
    

    // 记录到订单表
    sq_req_order req;
    memset(&req, 0, sizeof(req));
   
    req.contract = contract;
    req.direction = direction;
    req.market = market;
    req.offset = offset;
    req.price = price;
    req.product_type = product_type;
    req.qty = qty;
    req.sq_local_id = local_id;
    req.type = order_type;

    int ret = sq_mdb_put(tid_place_order, (char *)&req, sizeof(req));
    if (ret != ok)
    {
        return ret;
    }
    // 发送给插件
    if (m_td_plugs.empty())
    {
        return err_fail;
    }
    ret = m_td_plugs[0]->plug_put(tid_place_order, (char *)&req, sizeof(req));
    if (ret != ok)
    {
        return ret;
    }
    return ok;
}


int buy_open(int local_id, const char *contract, double price, int qty)
{
    return place_order(local_id, "", contract, f_buy, f_open, price, qty, 0, 0);
}

int sell_open(int local_id, const char *contract, double price, int qty)
{
    return place_order(local_id, "", contract, f_sell, f_open, price, qty, 0, 0);

}
int buy_close(int local_id, const char *contract, double price, int qty)
{
    return place_order(local_id, "", contract, f_buy, f_close, price, qty, 0, 0);

}
int sell_close(int local_id, const char *contract, double price, int qty)
{
    return place_order(local_id, "", contract, f_sell, f_close, price, qty, 0, 0);

}

int cancel_order(int local_id)
{
    sq_order_record *order = sq_mdb_get_order_by_id(local_id);
    if (order)
    {
        sq_req_cancel_order req;
        req.contract = order->contract;
        req.market = order->market;
        req.sq_local_id = local_id;
        req.user_id = order->user_id;
        return m_td_plugs[0]->plug_put(tid_cancel_order, (char *)&req, sizeof(req));
    }
    else
    {
        return err_not_exist;
    }
    // req.contract
}
void cancel_all_order(int direction)
{
    if (m_order_table == nullptr)
    {
        m_order_table = (sq_order_table*)get_table("order_table");
    }

    auto b = m_order_table->m_Index.begin();
    auto e = m_order_table->m_Index.end();
    auto it = b;
    for (; it != e; ++it)
    {
        if (it->second->is_finish == false)
        {
            if (it->second->direction == direction)
            {
                sq_req_cancel_order req;
                req.contract = it->second->contract;
                req.market = it->second->market;
                req.sq_local_id = it->second->sq_local_id;
                
                m_td_plugs[0]->plug_put(tid_cancel_order, (char *)&req, sizeof(req));
               
            }
        }
    }       
}
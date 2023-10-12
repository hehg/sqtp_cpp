#include "sq_app_base.h"
#include "os/sq_path.h"
#include "base/sq_getopt.h"
#include <signal.h>
#include <mutex>
#include "time/date_time.h"
#include "log/sq_logger.h"
#include "text/sq_config.h"
#include "text/sq_option_parser.h"
#include "sq_frame.h"
#include "sq_mdb.h"
#include "sq_tid_define.h"
#include "sq_struct_def.h"
namespace sq
{
	struct trade_msg_data{
		int tid;
		int size;
		char data[512];
	};
	static sq_app_base *s_cur_app = nullptr;
	static sq_config 		m_config;				//解析配置文件
	static string			m_app_path;				//应用程序路径
	static	string			m_app_name;
	static	int				m_app_id=0;
	static	string			m_app_dir;
	static	string 			m_conf_path;            //配置文件的位置
    static  string			m_trade_date="";		//交易日

	//static  std::mutex m_lock;
	
	extern logger s_log;
	void my_signal_fun(int signal_id)
	{
		SQ_LOG_FLUSH();
		printf("capture signal %d\n", signal_id);
		
		s_cur_app->on_close(signal_id);
		exit(0);
	}
	
	void sq_app_base::show_help()
	{
		printf("usage: ./app [-t trade_date] [-l log_level] [-c config path]\n");
	}
	sq_app_base::sq_app_base()
	{
		s_cur_app = this;
		date_time time =date_time::now();
		m_trade_date = time.format("YYYYMMDD");
		m_conf_path = "../conf/config.cfg";
	}

	sq_app_base::~sq_app_base()
	{
		
	}
	void sq_app_base::init(int argc, char **argv)
	{
		sq_option_parser m_option;				//解析命令行
		//=======设置解析命令行参数===========//
		m_option.add_option("-h", "--help", "shwo help info");
		m_option.add_option("-d", "--date", "trade date");
		m_option.add_option("-l", "--level", "", nullptr);
		m_option.add_option("-c", "--config", "config file path");
		m_option.add_option("-o", "--console", "ouput log to console");
		m_option.parser(argc, argv);

		if (m_option.has_opt("-d"))
		{
			m_trade_date = m_option.get_value_string("-d");
		}
		if (m_option.has_opt("-h"))
		{
			show_help();
		}
		m_app_path = argv[0];
		sq_path path(m_app_path.c_str());
		m_app_name = path.get_name();
#ifdef WINDOWS
		m_app_dir = string(path.get_driver()) + ":" + string(path.get_dir());
#else
		m_app_dir = string(dirname((char *)m_app_path.c_str()));
#endif
		//===========加载配置信息========//
		if (m_option.has_opt("-c"))
		{
			m_conf_path = m_option.get_value_string("-c");
		}
		m_config.open_file(m_conf_path.c_str());
		
		m_app_id = m_config.get_int("app_id");

		//======初始化日志=============//
		level_enum log_level = (level_enum)m_config.get_int("log_level", log_debug);
		if (m_option.has_opt("-l"))
		{
			log_level = (level_enum)m_option.get_value_int("-l");
		}
		string log_name = m_config.get_string("log_name", "app");

		string log_path = "../log/" + log_name + "_" + m_trade_date + ".log";

		if (log_name == "stdout")
		{
			s_log.add_stdout_target();
		}
		else
		{
			// 如果指定了这个参数，往控制台输出一份
			if (m_option.has_opt("-o"))
			{
				s_log.add_stdout_target();
			}
			s_log.add_file_target(log_path.c_str());
		}

		s_log.set_level(log_level);

		printf("EXE:%s\n", m_app_path.c_str());
		printf("app_id:%d\n", m_app_id);
		printf("trade_date:%s\n", m_trade_date.c_str());

		//=====启动信号捕获=====
		capture_signal();
	}
	
	
	void sq_app_base::run()
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		
	}
	
	void sq_app_base::capture_signal()
	{
		
		signal(SIGINT, my_signal_fun);
		signal(SIGILL, my_signal_fun);
#ifdef WINDOWS
		signal(SIGBREAK, my_signal_fun);
#endif
		signal(SIGABRT, my_signal_fun);
		signal(SIGTERM, my_signal_fun);
		//signal(SIGUSR1, my_signal_fun);

		// std::set_terminate();
	}
	const char* sq_app_base::get_app_path()
	{
		return m_app_path.c_str();
	}
	const char* sq_app_base::get_app_name()
	{
		return m_app_name.c_str();
	}
	const char* sq_app_base::get_app_folder()
	{
		return m_app_dir.c_str();
	}
}
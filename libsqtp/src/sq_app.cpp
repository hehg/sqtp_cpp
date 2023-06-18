#include "sq_app.h"
#include "os/sq_path.h"
#include "text/sq_config.h"
#include "os/sq_path.h"
#include "log/sq_logger.h"
#include "time/date_time.h"

namespace sq
{
    sq_app::sq_app(/* args */)
    {
        m_today=date_time::now().format("YYYYMMDD");
    }
    
    sq_app::~sq_app()
    {
    }

    void sq_app::init(int argc, char **argv)
	{
		
		m_app_path = argv[0];
		sq_path path(m_app_path.c_str());
		m_app_name = path.get_name();
#ifdef WINDOWS
		m_app_dir = string(path.get_driver()) + ":" + string(path.get_dir());
#else
		m_app_dir = string(dirname((char *)m_app_path.c_str()));
#endif
		//===========加载配置信息========//
		m_config.open_file(m_conf_path.c_str());
		
		m_app_id = get_config_int("app_id");

		//======初始化日志=============//
		level_enum log_level = (level_enum)get_config_int("log_level", log_debug);
		
		string log_name = get_config_string("log_name", "app");

		string log_path = "../log/" + log_name + "_" + m_today + ".log";

		if (log_name == "stdout")
		{
			s_log.add_stdout_target();
		}
		else
		{
			s_log.add_file_target(log_path.c_str());
		}

		s_log.set_level(log_level);

	}

    string sq_app::get_config_string(const char *key, const char* default_val)
	{
		return m_config.get_string(key,default_val);
	}
	int sq_app::get_config_int(const char *key, int default_val)
	{
		return m_config.get_int(key,default_val);
	}
	double sq_app::get_config_double(const char *key, double default_val)
	{
		return m_config.get_double(key,default_val);
	}
	int sq_app::get_config_array(const char *key, vector<string> &values)
	{
		return m_config.get_string_array(key,values);
	}
	int sq_app::get_config_array_int(const char *key, vector<int> &values)
	{
		return m_config.get_int_array(key,values);
	}
	bool sq_app::get_config_bool(const char *key, bool default_val)
	{
		return m_config.get_bool(key,default_val);
	}
	const char* sq_app::get_app_path()
	{
		return m_app_path.c_str();
	}
	const char* sq_app::get_app_name()
	{
		return m_app_name.c_str();
	}
	const char* sq_app::get_app_folder()
	{
		return m_app_dir.c_str();
	}
}
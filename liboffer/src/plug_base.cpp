#include "plug_base.h"
#include <string>
#include "log/sq_logger.h"
#include "time//date_time.h"
using namespace sq;

namespace sq_plug
{
    int plug_base::open()
    {
        //插件名称
        std::string name = get_cfg_string("name","");
        if(!name.empty()){
            m_name=name;
        }
        //=====日志=====
		{
             date_time dt=date_time::now();
			string log_level = get_cfg_string("log_level");
			if (!log_level.empty())
			{
				SQ_LOG_LEVEL((sq::level_enum)to_log_level(log_level.c_str()));
			}
			string log_name = get_cfg_string("log_name");
			if (log_name == "stdout") {
				s_log.add_stdout_target();
			}
            else if(log_name=="")
            {
				string today = dt.format("YYYYMMDD");
				string log_path = "./" + m_name + "_" + today + ".log";
				s_log.add_file_target(log_path.c_str());
            }
			else 
            {
				string today = dt.format("YYYYMMDD");
				string log_path = "./" + log_name + "_" + today + ".log";
				s_log.add_file_target(log_path.c_str());
			}
			
		}
        return 0;
    }
    std::string plug_base::get_cfg_string(std::string key, const std::string &def)
    {
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            return it->second;
        }
        else
        {
            return def;
        }
    }
    bool plug_base::get_cfg_bool(std::string key, bool def)
    {
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            if (it->second == "true")
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return def;
        }
    }
    int plug_base::get_cfg_int(std::string key, int def)
    {
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            return atoi(it->second.c_str());
        }
        else
        {
            return def;
        }
    }
    double plug_base::get_cfg_double(std::string key, double def)
    {
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            return atof(it->second.c_str());
        }
        else
        {
            return def;
        }
    }
    std::vector<std::string> plug_base::get_cfg_arrary(std::string key, char split )
    {
        using namespace sq;
        std::vector<std::string> out;
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            std::string val = it->second;
            std::string servers = sq_string_trim(val);
            sq_split(servers.c_str(), out, split);
        }
        return out;
    }
    std::vector<int> plug_base::get_cfg_arrary_int(std::string key, char split )
    {
        using namespace sq;
        std::vector<std::string> out;
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            std::string val = it->second;
            std::string servers = sq_string_trim(val);
            sq_split(servers.c_str(), out, split);
        }
        std::vector<int> rets;
        for (int i = 0; i < out.size(); i++)
        {
            rets.push_back(atoi(out[i].c_str()));
        }
        return rets;
    }
    int plug_base::set_option(const char *key, void *val)
    {
        if (strcmp(key, PLUG_OPT_id) == 0)
        {
            m_id = *(int *)val;
        }
        else if (strcmp(key, PLUG_OPT_plug_name) == 0)
        {
            m_name = (char *)val;
        }
        else if (strcmp(key, PLUG_OPT_callback) == 0)
        {
            m_call_back = (plug_callback_func_t)val;
        }
        else if (strcmp(key, PLUG_OPT_callback_param) == 0)
        {
            m_call_back_param = val;
        }
        else
        {
            m_options[key] = std::string((char *)val);
        }
        return ok;
    }

}
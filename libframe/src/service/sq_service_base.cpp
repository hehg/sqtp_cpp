#include "sq_service_base.h"

namespace sq
{
    std::unordered_map<std::string, std::function<sq_service_base *()>> Reflectable::s_classRegistry;

    sq_service_base *sq_service_base::create(const std::string &typeName)
    {
        auto it = s_classRegistry.find(typeName);
        if (it != s_classRegistry.end())
        {
            return it->second();
        }
        return nullptr;
    }
    void sq_service_base::registerClass(const std::string &typeName, std::function<sq_service_base *()> createFunc)
    {
        s_classRegistry[typeName] = createFunc;
    }

    std::string sq_service_base::get_cfg_string(std::string key, const std::string &def)
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
    bool sq_service_base::get_cfg_bool(std::string key, bool def)
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
    int sq_service_base::get_cfg_int(std::string key, int def)
    {
        return get_cfg<int>(key,def);
        
    }
    double sq_service_base::get_cfg_double(std::string key, double def)
    {
        return get_cfg<double>(key,def);
    }
    std::vector<std::string> sq_service_base::get_cfg_arrary(std::string key, char split )
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
    std::vector<int> sq_service_base::get_cfg_arrary_int(std::string key, char split )
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
    int sq_service_base::set_option(const char *key, void *val)
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
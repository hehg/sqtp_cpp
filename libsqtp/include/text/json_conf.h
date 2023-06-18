#pragma once
#include "sq_define.h"
#include "json.hpp"
namespace sq
{
    class json_conf
    {
    public:
        json_conf();
        ~json_conf();

        void load(const char *path);
        void load_str(const char * str);
		void load_js(nlohmann::json&js) { m_key_val = js; };
        string get_string(const char*key, string default_val = "");
        int get_string_array(const char*key, vector<string> &result);
        template<typename T>
        int get_array(const char*key, vector<T> &result)
        {
            if (m_key_val.find(key) != m_key_val.end())
            {
                result = m_key_val[key].get<vector<T>>();
                return result.size();
            }
            else
            {
                return 0;
            }
        }

        int    get_int(const char*key, int default_val = 0);
        double    get_double(const char*key, double default_val = 0);
        bool   get_bool(const char*key, bool default_val = true);

        nlohmann::json m_key_val;
    };
}

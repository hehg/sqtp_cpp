#include "text/json_conf.h"
#include "text/json.hpp"
#include <fstream>
namespace sq {
    json_conf::json_conf()
    {
    }


    json_conf::~json_conf()
    {
    }
    void json_conf::load(const char *path)
    {
        using namespace nlohmann;
        std::ifstream stream;
        stream.open(path, ios::in);
        if (stream.is_open() == true) {
            m_key_val = json::parse(stream);
        }
        else {
            cout << "open json conf fail:path=" <<path <<endl;
            throw (std::invalid_argument ("open json conf fail"));
        }
        
    }

    void json_conf::load_str(const char * str)
    {
        using namespace nlohmann;
        m_key_val=json::parse(str);
    }
    
    string json_conf::get_string(const char*key, string default_val)
    {
        if (m_key_val.find(key) != m_key_val.end())
        {
            return m_key_val[key].get<std::string>();
        }
        else
        {
            return default_val;
        }
    }
    int json_conf::get_string_array(const char*key, vector<string> &result)
    {
        if (m_key_val.find(key) != m_key_val.end())
        {
            result=m_key_val[key].get<vector<string>>();
            return result.size();
        }
        else
        {
            return 0;
        }
    }
    int  json_conf::get_int(const char*key, int default_val)
    {

        if (m_key_val.find(key) != m_key_val.end())
        {
            return  m_key_val[key].get<int>();
        }
        else
        {
            return default_val;
        }
    }

    double  json_conf::get_double(const char*key, double default_val)
    {
        if (m_key_val.find(key) != m_key_val.end())
        {
            return  m_key_val[key].get<double>();
        }
        else
        {
            return default_val;
        }
    }
    bool   json_conf::get_bool(const char*key, bool default_val)
    {
        if (m_key_val.find(key) != m_key_val.end())
        {
            return  m_key_val[key].get<bool>();
        }
        else
        {
            return default_val;
        }
    }
}

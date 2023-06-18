#pragma once
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "plug/sq_plug.h"
#include "text/sq_string.h"
#include "error/sq_err.h"
using namespace sq;

namespace sq_plug
{
    struct bad_from_string : bad_cast {
    const char* what() const noexcept override { // 这里的函数签名视编译器而定
        return "bad cast from string";
    }
};

template<typename T>
T from_string(const string& s) {
    istringstream is(s);
    T t;
    if (!(is >> t)) throw bad_from_string();
    return t;
}
    class plug_base
    {
    public:
        int open();

        std::string get_cfg_string(std::string key, const std::string &def = "");

        bool get_cfg_bool(std::string key, bool def = true);

        int get_cfg_int(std::string key, int def = 0);

        double get_cfg_double(std::string key, double def = 0);

        std::vector<std::string> get_cfg_arrary(std::string key, char split = ',');

        std::vector<int> get_cfg_arrary_int(std::string key, char split = ',');

        template <typename T>
        std::set<T> get_cfg_set(std::string key, char split = ',')
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
            std::set<T> rets;
            for (int i = 0; i < out.size(); i++)
            {
                rets.insert(from_string<T>(out[i]));
            }
            return rets;
        }

        int set_option(const char *key, void *val);

        // 基础成员
        int m_id = 0;
        std::string m_name;
        std::unordered_map<std::string, std::string> m_options;
        plug_callback_func_t m_call_back = nullptr;
        void *m_call_back_param = nullptr;
    };
}
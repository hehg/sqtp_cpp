#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "text/sq_string.h"
namespace sq
{

    /**
     * @brief 服务的基类
     */
    class sq_service_base
    {
    public:
        virtual int open() = 0;
        virtual void close() = 0;
        virtual ~sq_service_base() = default;

        virtual int set_option(const char *key, void *val);
       
        sq_service_base() = default;
        sq_service_base(const sq_service_base &) = delete;
        // 反射相关
        virtual std::string get_type_name() const = 0;
        static sq_service_base *create(const std::string &typeName);
        static void registerClass(const std::string &typeName, std::function<sq_service_base *()> createFunc);
        
        static std::unordered_map<std::string, std::function<sq_service_base *()>> s_classRegistry;

        //===配置信息相关 ===//
        template<typename T>
        T& get_cfg(const char *key, const T&def) const
        {
            auto it = m_cfg.find(key);
            if (it!= m_cfg.end())
            {
                return sq_from_string(it->second);
            }
            return def;
        }

        bool get_cfg_bool(std::string key, bool def = true);

        int get_cfg_int(std::string key, int def = 0);

        double get_cfg_double(std::string key, double def = 0);

        std::vector<std::string> get_cfg_arrary(std::string key, char split = ',');

        std::vector<int> get_cfg_arrary_int(std::string key, char split = ',');

        /**
         * 获取配置信息，返回 set
         */
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

        std::unordered_map<std::string, std::string> m_options; // 配置项
    };

#define DECLARE_SERVER(className)                       \
public:                                              \
    virtual std::string get_type_name() const override \
    {                                                \
        return #className;                           \
    }                                                \
    static sq_service_base *create()                     \
    {                                                \
        return new className();                      \
    }

#define REGISTER_SERVER(className)                                          \
    namespace                                                              \
    {                                                                      \
        struct Register##className                                         \
        {                                                                  \
            Register##className()                                          \
            {                                                              \
                sq_service_base::registerClass(#className, className::create); \
            }                                                              \
        } register##className;                                             \
    }
}
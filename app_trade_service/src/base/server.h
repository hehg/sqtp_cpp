#pragma once
/**
 * 所有服务的基类
 */
#include "pack/sq_package.h"
#include <unordered_map>
#include <set>
#include <string>
#include "text/sq_string.h"

using namespace sq;


namespace sq
{
    class ws_channel;
}
class server;
struct server_cmd
{
    int from_server = 0;
    int target_server = 0;
    int tid = 0;
    sq::sq_package::ptr pkg = nullptr;
    sq::ws_channel *from_channel = nullptr;
};
using post_funct = std::function<void(server_cmd *cmd)>;
typedef server*(*server_create_funct)();

struct server
{
    enum ID
    {
        gate_server = 1,
        query_server = 2,
        data_server = 3,
        md_server = 4,
    };
    virtual int open(const char *cfg_path) = 0;
    virtual int start() { return 0; };
    virtual int stop() { return 0; };
    virtual int put(const char *data, int len) { return 0; };
    virtual int get(const char *data, int len) { return 0; };
    virtual int set_option(const char *key, void *val);
    virtual int on_msg(int tid,sq_package::ptr pkg,sq::ws_channel*from_channel){return 0;}

    static int post_msg(int tid,sq_package::ptr pkg,sq::ws_channel*from_channel)
    {
         auto it = s_subscribe_mgr.find(tid);
         if(it!=s_subscribe_mgr.end())
         {
             for(auto &s:it->second)
             {
                s->on_msg(tid,pkg,from_channel);
             }
         }
         return 0;
    }
    
    inline int id() const { return m_id; }
    server() = default;
    server(const server &) = delete;
    // 反射相关
    virtual std::string get_type_name() const = 0;
    static server *create(const std::string &typeName);
    static void registerClass( const char*typeName, server_create_funct createFunc);

    //订阅相关
    static std::unordered_map<int, std::list<server*>> s_subscribe_mgr;
    static void subscribe(int id,server*s)
    {

        auto it = s_subscribe_mgr.find(id);
        if(it == s_subscribe_mgr.end()){
            s_subscribe_mgr[id] = std::list<server*>{s};
        }
        else{
            it->second.push_back(s);
        }
        
    }

    //===配置信息相关 ===//
    template <typename T>
    T get_cfg(const string &key, const T &default_val) const
    {
        auto it = m_options.find(key);
        if (it != m_options.end())
        {
            return sq_from_string<T>(it->second);
        }
        return default_val;
    }
    std::string get_cfg_string(std::string key, const std::string &default_val);

    bool get_cfg_bool(std::string key, bool default_val = true);

    int get_cfg_int(std::string key, int default_val = 0);

    double get_cfg_double(std::string key, double default_val = 0);

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
            rets.insert(sq_from_string<T>(out[i]));
        }
        return rets;
    }

    post_funct m_post_funct = nullptr;
    int m_id = 0;
    std::unordered_map<std::string, std::string> m_options; // 配置项
};

#define DECLARE_SERVER(className)                       \
public:                                              \
    virtual std::string get_type_name() const override \
    {                                                \
        return #className;                           \
    }                                                \
    static server *create()                     \
    {                                                \
        return new className();                      \
    }

#define REGISTER_SERVER(className)                                          \
    {                                                                      \
        struct Register##className                                         \
        {                                                                  \
            Register##className()                                          \
            {                                                              \
                server::registerClass(#className, className::create); \
            }                                                              \
        }register##className;        \
    }


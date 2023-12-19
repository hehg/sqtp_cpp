#include "server.h"
#include "error/sq_err.h"
using namespace sq;

std::map<std::string, server_create_funct> s_classRegistry={};
std::unordered_map<int, std::list<server*>> server::s_subscribe_mgr;

server *server::create(const std::string &typeName)
{
    auto it = s_classRegistry.find(typeName);
    if (it != s_classRegistry.end())
    {
        return it->second();
    }
    return nullptr;
}
void server::registerClass( const char*typeName, server_create_funct createFunc)
{
    s_classRegistry[typeName] = createFunc;
}

std::string server::get_cfg_string(std::string key, const std::string &default_val)
{
    auto it = m_options.find(key);
    if (it != m_options.end())
    {
        return it->second;
    }
    else
    {
        return default_val;
    }
}
bool server::get_cfg_bool(std::string key, bool default_val)
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
        return default_val;
    }
}
int server::get_cfg_int(std::string key, int default_val)
{
    return get_cfg<int>(key, default_val);
}
double server::get_cfg_double(std::string key, double default_val)
{
    return get_cfg<double>(key, default_val);
}
std::vector<std::string> server::get_cfg_arrary(std::string key, char split)
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
std::vector<int> server::get_cfg_arrary_int(std::string key, char split)
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
int server::set_option(const char *key, void *val)
{

    m_options[key] = std::string((char *)val);

    return ok;
}
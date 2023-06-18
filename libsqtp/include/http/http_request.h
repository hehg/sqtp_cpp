#pragma once
#include <string>
#include <map>
#include <string.h>

using namespace std;
namespace sq
{
    /** 解析http 请求
     */
    class http_request
    {
    public:
        enum HttpMethod
        {
            UNKNOWN,
            GET,
            POST,
            PUT,
            DELETE,
            HEAD
        };
        http_request()
        {
        }
        const char* get_method(){return m_method.c_str();}
        const char* get_head(const char*key){
            auto it=m_headers.find(key);
            if(it!=m_headers.end()){
                return it->second.c_str();
            }
            else{
                return nullptr;
            }
        }
        std::string m_method;
        std::string m_url;
        std::string m_protocol_name;
        std::string m_protocol_version;
        std::map<string, string> m_headers;
        int         m_body_size     = 0;
        const char  *m_body_begin   = nullptr;

        void dump();
        /**
         * 解析 http数据，
         * @return -1 解析失败
         * @return 0 是一个不完整的协议
         * @return >0 成功 解析了多少字节
         */
        int parser_http(const char *data, int size);
    };

}
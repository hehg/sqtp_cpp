#pragma once
#include <string>
#include <map>
#include <string.h>

using namespace std;
namespace sq
{
    class http_response
    {
    public:
        http_response()
        {
        }
        void dump(); 
        /**
         * @brief 解析 http 信息
         * @return 解析的字节数
        */
        int parser_http(const char *data, int size);

        inline int get_status_code(){return std::atoi(m_status_code.c_str());}
        const char* get_status_reason(){return m_status_reasons.c_str();}
        const char* get_head(const char*key){
            auto it=m_headers.find(key);
            if(it!=m_headers.end()){
                return it->second.c_str();
            }
            else{
                return nullptr;
            }
        }

        std::string m_protocol_name;
        std::string m_protocol_version;
        std::string m_status_code;
        std::string m_status_reasons;
        map<string, string> m_headers;
        int         m_body_size     = 0;
        const char  *m_body_begin   = nullptr;

        
            
    };

}
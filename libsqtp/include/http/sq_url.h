#pragma once
#include <string>

namespace sq
{
    class sq_url
    {
        public:
        int parser(const char* url);

        std::string m_protocol;
        std::string m_host;
        std::string m_port;
        std::string m_path;
        std::string m_param;
        std::string m_anchor;
    };
}
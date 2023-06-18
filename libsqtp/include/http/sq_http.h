#pragma once

namespace sq
{
    #define HTTP_VERSION "HTTP/1.1"

    enum http_method
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
}
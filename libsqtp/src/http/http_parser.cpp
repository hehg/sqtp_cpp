#include <iostream>
#include <string.h>
#include "http/http_request.h"
#include "http/http_response.h"
#include "text/sq_string.h"

using namespace std;
namespace sq
{
    static bool parse_head(const char *line, const char *end, string &key, string &val)
    {
        const char *p = line;
        int step = 1;
        while (p <= end)
        {
            if (step == 1)
            {
                if (*p == ':')
                {
                    for (; *(p + 1) == ' '; ++p)
                    {
                    }
                    step = 2;
                }
                else
                {
                    key.push_back(*p);
                }
            }
            else if (step == 2)
            {
                val.push_back(*p);
            }
            ++p;
        }
        to_lower(key);
        if (step == 2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    void http_request::dump()
    {
        cout << "action=" << m_method << std::endl;
        cout << "uri=" << m_url << std::endl;
        cout << "protocol=" << m_protocol_name << std::endl;
        cout << "protocol_ver=" << m_protocol_version << std::endl;

        for (auto a : m_headers)
        {
            cout << a.first << "=" << a.second << "\n";
        }
        cout << "body_size=" << m_body_size << std::endl;
        cout << "body=" << m_body_begin;
    }

    int http_request::parser_http(const  char *data, int size)
    {
        m_headers.clear();
        m_method = "";
        m_protocol_name = "";
        m_protocol_version = "";
        m_url = "";

        // 用有限状态机解析 HTTP 请求
        enum State
        {
            REQUEST_LINE_START, // 开始解析首行
            REQUEST_LINE_METHOD,
            REQUEST_LINE_SPACE_BEFORE_URI,
            REQUEST_LINE_URI,
            REQUEST_LINE_SPACE_BEFORE_PROTOCOL,
            REQUEST_LINE_PROTOCOL,
            REQUEST_LINE_PROTOCOL_VER,
            REQUEST_LINE_CR,
            REQUEST_LINE_LF,

            HEADER_LINE_START, // 开始解析头
            HEADER_LINE_KEY,
            HEADER_LINE_COLON,
            HEADER_LINE_SPACE_BEFORE_VALUE,
            HEADER_LINE_VALUE,
            HEADER_LINE_CR,
            HEADER_LINE_LF,
            HEADER_LINE_DONE,

            BODY
        } state = REQUEST_LINE_START;

        string key, val;
        for (int i = 0; i < size; i++)
        {
            char ch = *(data + i);

            switch (state)
            {
            case REQUEST_LINE_START:
            {
                if (ch == '\r' || ch == '\n')
                    break;
                state = REQUEST_LINE_METHOD;
                m_method += ch;
                break;
            }
            case REQUEST_LINE_METHOD:
            {
                if (ch == ' ')
                {
                    state = REQUEST_LINE_SPACE_BEFORE_URI;
                }
                else
                {
                    m_method += ch;
                }
                break;
            }

            case REQUEST_LINE_SPACE_BEFORE_URI:
            {
                if (ch == '/')
                {
                    state = REQUEST_LINE_URI;
                    m_url += ch;
                }
                break;
            }
            case REQUEST_LINE_URI:
            {
                if (ch == ' ')
                {
                    state = REQUEST_LINE_SPACE_BEFORE_PROTOCOL; // 开始解析协议
                }
                else
                {
                    m_url += ch;
                }
                break;
            }
            case REQUEST_LINE_SPACE_BEFORE_PROTOCOL:
            {
                if (ch == 'H')
                {
                    m_protocol_name += ch;
                    state = REQUEST_LINE_PROTOCOL;
                }
                else
                {
                    m_protocol_name += ch;
                }
                break;
            }
            case REQUEST_LINE_PROTOCOL:
            {
                if (ch == '/')
                {
                    state = REQUEST_LINE_PROTOCOL_VER;
                }
                else
                {
                    m_protocol_name += ch;
                }
                break;
            }
            case REQUEST_LINE_PROTOCOL_VER:
            {
                if (ch == '\r')
                {
                    state = REQUEST_LINE_LF;
                }
                else
                {
                    m_protocol_version += ch;
                }
                break;
            }
            case REQUEST_LINE_LF:
            {
                if (ch == '\n')
                {
                    state = HEADER_LINE_START;
                }
                break;
            }
            case HEADER_LINE_START:
            {
                if (ch == '\r')
                {
                    state = HEADER_LINE_DONE;
                    break;
                }

                state = HEADER_LINE_KEY;
                key += tolower(ch);
                break;
            }
            case HEADER_LINE_KEY:
            {
                if (ch == ':')
                {
                    state = HEADER_LINE_COLON;
                }
                else
                {
                    key += tolower(ch);
                }
                break;
            }
            case HEADER_LINE_COLON:
            {
                if (ch == ' ')
                {
                    state = HEADER_LINE_SPACE_BEFORE_VALUE;
                }
                break;
            }
            case HEADER_LINE_SPACE_BEFORE_VALUE:
            {
                if (ch != ' ')
                {
                    state = HEADER_LINE_VALUE;
                    val += ch;
                }
                break;
            }
            case HEADER_LINE_VALUE:
            {
                if (ch == '\r')
                {
                    state = HEADER_LINE_CR;
                }
                else
                {
                    val += ch;
                }
                break;
            }
            case HEADER_LINE_CR:
            {
                if (ch == '\n')
                {
                    state = HEADER_LINE_START;
                    m_headers[key] = val;
                    key = val = "";
                }
                break;
            }
            case HEADER_LINE_DONE:
            {
                if (ch == '\n')
                {
                    state = BODY;
                }
                break;
            }
            case BODY:
            {
                m_body_begin = data + i;
                m_body_size = size - i;
                // finish
                return size;
            }

            default:
                return -1;
            }
        }

        // check

        return 0;
    }

    //http 应答解析
    void http_response::dump()
    {
        cout << "protocol=" << m_protocol_name << std::endl;
        cout << "m_protocol_version=" << m_protocol_version << std::endl;
        cout << "m_status_code=" << m_status_code << std::endl;
        cout << "m_status_reasons=" << m_status_reasons << std::endl;
        cout << "m_body_size=" << m_body_size<<std::endl;

        for (auto a : m_headers)
        {
            cout << a.first << "=" << a.second << "\n";
        }
        
    }
    //基于状态机解析http 应答
    int http_response::parser_http(const char *data, int size)
    {
        m_headers.clear();
        m_status_code = "";
        m_protocol_name = "";
        m_protocol_version = "";
        m_status_reasons = "";

        /*
        HTTP/1.0 200 OK
        Server: SimpleHTTP/0.6 Python/3.8.2
        Date: Mon, 08 Nov 2021 11:59:28 GMT
        Content-type: text/html; charset=utf-8
        Content-Length: 673
        */
        // 用有限状态机解析 HTTP 请求
        enum State
        {
            RSP_LINE_START, // 开始解析首行
            RSP_LINE_PROTOCOL,
            RSP_LINE_PROTOCOL_VER,
            RSP_LINE_SPACE_BEFORE_CODE,
            RSP_LINE_CODE,
            RSP_LINE_SPACE_BEFORE_REASON,
            RSP_LINE_REASON,
            RSP_LINE_CR,
            RSP_LINE_LF,

            HEADER_LINE_START, // 开始解析头
            HEADER_LINE_KEY,
            HEADER_LINE_COLON,
            HEADER_LINE_SPACE_BEFORE_VALUE,
            HEADER_LINE_VALUE,
            HEADER_LINE_CR,
            HEADER_LINE_LF,
            HEADER_LINE_DONE,

            BODY
        } state = RSP_LINE_START;

        string key, val;
        for (int i = 0; i < size; i++)
        {
            char ch = *(data + i);

            switch (state)
            {
            case RSP_LINE_START:
            {
                if (ch == '\r' || ch == '\n')
                    break;
                state = RSP_LINE_PROTOCOL;
                m_protocol_name += ch;
                break;
            }
            case RSP_LINE_PROTOCOL:
            {
                if (ch == '/')
                {
                    state = RSP_LINE_PROTOCOL_VER;
                }
                else
                {
                    m_protocol_name += ch;
                }
                break;
            }
            case RSP_LINE_PROTOCOL_VER:
            {
                if (ch == ' ')
                {
                    state = RSP_LINE_SPACE_BEFORE_CODE;
                }
                else
                {
                    m_protocol_version += ch;
                }
                break;
            }

            case RSP_LINE_SPACE_BEFORE_CODE:
            {
                if(ch!=' '){
                    state=RSP_LINE_CODE;
                    m_status_code+=ch;
                }
                break;
            }
            case RSP_LINE_CODE:
            {
                if (ch ==' ')
                {
                    state = RSP_LINE_SPACE_BEFORE_REASON;
                }
                else{
                    m_status_code+=ch;
                }
                break;
            }
            case RSP_LINE_SPACE_BEFORE_REASON:
            {
                if(ch!=' '){
                    state = RSP_LINE_REASON;
                     m_status_reasons+=ch;
                }
                break;
            }

            case RSP_LINE_REASON:
            {
                if (ch == '\r')
                {
                    state = RSP_LINE_LF;
                }
                else
                {
                    m_status_reasons += ch;
                }
                break;
            }

            case RSP_LINE_LF:
            {
                if(ch=='\n'){
                    state=HEADER_LINE_START;
                }
                break;
            }
            
            
            case HEADER_LINE_START:
            {
                if (ch == '\r')
                {
                    state = HEADER_LINE_DONE;
                    break;
                }

                state = HEADER_LINE_KEY;
                key += tolower(ch);
                break;
            }
            case HEADER_LINE_KEY:
            {
                if (ch == ':')
                {
                    state = HEADER_LINE_COLON;
                }
                else
                {
                    key += tolower(ch);
                }
                break;
            }
            case HEADER_LINE_COLON:
            {
                if (ch == ' ')
                {
                    state = HEADER_LINE_SPACE_BEFORE_VALUE;
                }
                break;
            }
            case HEADER_LINE_SPACE_BEFORE_VALUE:
            {
                if (ch != ' ')
                {
                    state = HEADER_LINE_VALUE;
                    val += ch;
                }
                break;
            }
            case HEADER_LINE_VALUE:
            {
                if (ch == '\r')
                {
                    state = HEADER_LINE_CR;
                }
                else
                {
                    val += ch;
                }
                break;
            }
            case HEADER_LINE_CR:
            {
                if (ch == '\n')
                {
                    state = HEADER_LINE_START;
                    m_headers[key] = val;
                    key = val = "";
                }
                break;
            }
            case HEADER_LINE_DONE:
            {
                if (ch == '\n')
                {
                    state = BODY;
                }
                break;
            }
            case BODY:
            {
                m_body_begin = data + i;
                m_body_size = size - i;
                // finish
                return size;
            }

            default:
                return -1;
            }
        }

        // check

        return 0;
    }
}
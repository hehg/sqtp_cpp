#include "http/sq_url.h"
#include "sq_define.h"
namespace sq
{
    int sq_url::parser(const char *url)
    {
        using namespace std;
        string protocol;
        string host;
        string port;
        string path;
        string param;
        string anchor;

        char *p = (char *)url;
        int step = 0; //解析协议
        //step=1 host (必须)
        //step=2 端口
        //step=3 路径   必须
        //step=5 param
        //step=6 anchor
        while (*p != '\0')
        {
            if (step == 0) //解析协议
            {
                if (*p == ':' && *(p + 1) == '/' && *(p + 2) == '/')
                {
                    step = 1;
                    p += 2;
                }
                else
                {
                    protocol.push_back(*p);
                }
            }
            else if (step == 1) //解析host
            {
                if (*p == '/')
                {
                    step = 3;
                    path.push_back(*p);
                }
                else if (*p == ':')
                {
                    step = 2;
                }
                else if (*p == '?')
                {
                    step = 5;
                }
                else
                {
                    host.push_back(*p);
                }
            }
            else if (step == 2) //端口
            {
                if (*p == '/')
                {
                    step = 3;
                    path.push_back(*p);
                }
                else
                {
                    port.push_back(*p);
                }
            }
            else if (step == 3) //路径
            {
                if (*p == '?')
                {
                    step = 5;
                }
                else
                {
                    path.push_back(*p);
                }
            }
            else if (step == 5) //param
            {
                if (*p == '#')
                {
                    step = 6;
                }
                else
                {
                    param.push_back(*p);
                }
            }
            else if (step == 6) //anchor
            {
                anchor.push_back(*p);
            }
            ++p;
        }
        m_protocol=protocol;
        m_host=host;
        m_port=port;
        m_path=path;
        m_param=param;
        m_anchor=anchor;

        if(m_path==""){
            m_path="/";
        }
#if 1
        cout << "protocol=" << protocol << std::endl;
        cout << "host=" << host << std::endl;
        cout << "path=" << path << std::endl;
        cout << "param=" << param << std::endl;
        cout << "anchor=" << anchor << std::endl;
#endif
		return 0;
    }
}
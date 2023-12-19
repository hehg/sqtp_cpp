#include "http/http_client.h"
#include "net/tcp_client.h"
#include "net/reactor.h"
#include "http/sq_url.h"
#include "http/http_request.h"
#include "http/http_response.h"
namespace sq
{
    #if 0
    http_client::http_client()
    {
        sq_reactor*r=get_default_reactor();
        m_tcp=new tcp_client(r);
    }
    http_client::~http_client()
    {
        delete m_tcp;
        m_tcp=nullptr;
    }
     void http_client::get(const char *url_path)
     {
        
        // sq_url url;
        // url.parser(url_path);

        // char server_ip[128]={0};
        // sprintf(server_ip,"tcp://%s:%s",url.m_host.c_str(),url.m_port.c_str());

        // bool ret=m_tcp->connect(server_ip);
        // if(!ret){
        //     return;
        // }
        // using namespace std::placeholders;
        // m_tcp->get_channel()->reg_on_msg(std::bind(&http_client::on_message,this,_1,_2,_3));
        // http_request req;
        // req.set_host(url.m_host.c_str());
        // req.set_method(http_method::GET);
        // req.set_path(url.m_path.c_str());
        // string s=req.serialize();
        // rsp_state=0;
        // m_tcp->get_channel()->send_data(s.c_str(),s.length());
     }

     int http_client::on_message(void*package, int size,void*from)
     {
       char*p=(char*)package;
       char*begin=p;
       char*end=p+size;

       printf("size=%d,%s\n",size,p);
       //read request line
       string http;
       string code;
       string code_reason;
/*
HTTP/1.0 200 OK
Server: SimpleHTTP/0.6 Python/3.8.2
Date: Mon, 08 Nov 2021 11:59:28 GMT
Content-type: text/html; charset=utf-8
Content-Length: 673

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Directory listing for /</title>
</head>
<body>
<h1>Directory listing for /</h1>
<hr>
<ul>
<li><a href=".DS_Store">.DS_Store</a></li>
<li><a href="api/">api/</a></li>
<li><a href="bin/">bin/</a></li>
<li><a href="build.sh">build.sh</a></li>
<li><a href="CMakeLists.txt">CMakeLists.txt</a></li>
<li><a href="common/">common/</a></li>
<li><a href="conf/">conf/</a></li>
<li><a href="lib/">lib/</a></li>
<li><a href="obj/">obj/</a></li>
<li><a href="src/">src/</a></li>
</ul>
<hr>
</body>
</html>
*/
       
       for(int i=0;i<size;i++)
       {
         if(rsp_state==0){ //get rsp line 
             http.push_back(*p);
             if(*p==' '||*p=='\t'){
                 rsp_state=2;
             }
         }
         else if(rsp_state==2){
             code.push_back(*p);
             if(*p==' '){
                 rsp_state=3;
             }
         }
         else if(rsp_state==3){
             code.push_back(*p);
             if(*p=='\r'){
                 rsp_state=3;
             }
         }
         ++p;
       }
       return size;
     } 
     #endif 
}
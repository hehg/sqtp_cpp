#include "http/ws_handler.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include  "log/sq_logger.h"
#include "http/websocket.h"
#include "text/sq_string.h"
namespace sq
{
    ws_handler::ws_handler()
    {
    }
    ws_handler::ws_handler(event_reactor *r):tcp_handler(r)
    {
    }
    ws_handler::ws_handler(event_reactor *r, fd_t fd):tcp_handler(r, fd)
    {
        
    }
    
    //握手请求
    int ws_handler::handshake_request(char*msg,int len)
    {
        /* 客户端的握手协议如下
        GET /chat HTTP/1.1
        Host: server.example.com
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
        Origin: http://example.com
        Sec-WebSocket-Protocol: chat, superchat
        Sec-WebSocket-Version: 13

        */
        log_debug("websocket handshake request,msg={}\n",msg);
        char rsp_msg[1024]={0};
        int rsp_msg_size=0;
        http_request http_req;
        int ret= http_req.parser_http(msg, len);
        if(ret!=0){
            rsp_msg_size = sprintf(rsp_msg, "HTTP/1.1 403 Forbidden\r\nSec-WebSocket-Version: 13\r\n");
        }
        string rsp;
      
        
        //为GET 请求
        {
            compare_no_case(http_req.get_method(),"GET");
        }
        //http version is 1.1 or above
        {
            rsp="HTTP/1.1 101 Switching Protocols\r\n";
        }
        
        //host 
        {
            const char*val= http_req.get_head("host");
            if (!val)
            {
                log_info("not exit host\n");
                return err_invalid_package;
            }

        }
        //Upgrade is "websocket"
        {
            const char*val= http_req.get_head("upgrade");
            if (!val)
            {
                log_info("not exit Upgrade\n");
                return err_invalid_package;
            }
            if(compare_no_case(val,"websocket")==false){
                log_info("invalid Upgrade val,val={}\n",val);
                return err_invalid_package;
            }
            rsp+="Upgrade: websocket\r\n";
        }
        //Connection is "Upgrade"
        {
            const char*val= http_req.get_head("connection");
            if (!val)
            {
                log_info("not exit connection\n");
                return err_invalid_package;
            }
            if(compare_no_case(val,"Upgrade")==false){
                log_info("invalid Upgrconnectionade val,val={}\n",val);
                return err_invalid_package;
            }
            rsp+="Connection: Upgrade\r\n";
        }

        //Sec-WebSocket-Key check
        #if 1
        {
             const char*val= http_req.get_head("sec-websocket-key");
            if (!val)
            {
                log_info("not exit sec-websocket-key\n");
                return err_invalid_package;
            }
            int req_size=strlen(val);
            int rsp_size=req_size+36;
            char wskey[128]={0};
            memcpy(wskey,val,req_size);
            memcpy(wskey+req_size,WS_GUID,36);

            char accept_str[32];
            accept_str[sha1base64((uint8_t *)wskey, req_size + 36, accept_str)] = 0;
            
            char accetp_key[256]={0};
            sprintf(accetp_key,"Sec-WebSocket-Accept: %s\r\n",accept_str);
            log_debug("Sec-WebSocket-Accept={}\n",accetp_key);
            rsp+=accetp_key;
        }
        #endif
        //Sec-WebSocket-Version is 13
        {
             const char*val= http_req.get_head("sec-websocket-version");
            if (!val)
            {
                log_info("not exit Sec-WebSocket-Version\n");
                return err_invalid_package;
            }
            if(compare_no_case(val,"13")==false){
                log_info("invalid Sec-WebSocket-Version val,val={}\r\n",val);
                return err_invalid_package;
            }
        }

        //Origin :来自哪个浏览器 或 客户端

        //Sec-WebSocket-Protocol  支持的协议
        {
            const char*val= http_req.get_head("sec-websocket-protocol");
            if (val)
            {
               rsp+="Sec-WebSocket-Protocol: "+string(val)+"\r\n";
            }
        }
        //Sec-WebSocket-Extensions 扩展协议
        {
            const char*val= http_req.get_head("sec-websocket-extensions");
            if (val)
            {
               
            }
        }

        //握手成功后  
        m_state=1;
        rsp+="\r\n";
        /* rsp send to cleint
        HTTP/1.1 101 Switching Protocols
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
        Sec-WebSocket-Protocol: chat

        */    
        printf("websocket handshake rsp=%s\n",rsp.c_str());
        this->send_data(rsp.c_str(),rsp.size());
		return ok;
    }

    
}
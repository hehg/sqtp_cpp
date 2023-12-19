#include "http/ws_channel.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include  "log/sq_logger.h"
#include "http/websocket.h"
#include "text/sq_string.h"
#include <assert.h>
#include "time/date_time.h"
#include "crypto/sq_crypto.h"
#include "sq_macro.h"

namespace sq
{
    ws_channel::ws_channel()
    {
    }
    ws_channel::ws_channel(event_reactor *r):tcp_channel(r)
    {
    }
    ws_channel::ws_channel(event_reactor *r, fd_t fd):tcp_channel(r, fd)
    {
        
    }
    int ws_channel::send_ws_package(const char *data, uint32_t size, uint8_t opcode, bool fin)
    {
        if(m_ws_state!=ws_state_handshak_ok){
            printf("not connected can not send data\n");
            return err_fail;
        }
        assert(size+sizeof(ws_header)<sizeof(tmp_buf));
        
        ws_header *hdr = (ws_header *)tmp_buf;
        char* body=tmp_buf+sizeof(ws_header);

        if (fin)
        {
            hdr->fin = 1;
        }
        else
        {
            hdr->fin = 0;
        }
        hdr->rsv1 = 0;
        hdr->rsv2 = 0;
        hdr->rsv3 = 0;
        hdr->opcode = opcode;

        hdr->masked = 1;  //client must set as  1
        if (size <= 125)
        {
            hdr->payloadlen = size;
        }
        else if (size <= 0xFFFF)
        {
            hdr->payloadlen = 126;
            *(uint16_t *)(tmp_buf + sizeof(ws_header)) = htons(size);
            body+=2;
        }
        else
        {
            hdr->payloadlen = 127;
            *(uint64_t *)(tmp_buf + sizeof(ws_header)) = htonll(size);
            body+=8;
        }
        //set mask key
        mask_key_t mask;
        mask.integer=(uint32_t)(date_time::get_now_us()&0xffff);
        *((uint32_t*)body)=mask.integer;
        body+=sizeof(mask_key_t);
        if (hdr->masked)
        {
            for (uint32_t i = 0; i < size; i++)
            {
                *(body++) = data[i] ^ mask.char_array[i % 4];
            }
        }
        else{
            memcpy(body,data,size);
            body+=size;
        }

        int pkg_size=body-tmp_buf;
        int ret=send_data(tmp_buf,pkg_size);
        if(ok==ret){
            printf("send data ok ,body_size=%d\n",size);
        }
        else{
            return err_fail;
        }
        return ok;
    }
    //处理握手请求
    int ws_channel::handshake_request(char*msg,int len)
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
        m_ws_state=ws_state_handshak_ok;
        rsp+="\r\n";
        /* rsp send to cleint
        HTTP/1.1 101 Switching Protocols
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
        Sec-WebSocket-Protocol: chat

        */    
        log_debug("websocket handshake rsp={}\n",rsp.c_str());
        this->send_data(rsp.c_str(),rsp.size());
		return ok;
    }

    int ws_channel::handshake_response(char *msg, int size)
    {
        int count=0;
        //客户端收到握手应答
        if (m_ws_state == ws_channel::ws_state_handshaking) // 握手状态
        {
            /* rsp from server
            HTTP/1.1 101 Switching Protocols
            Upgrade: websocket
            Connection: Upgrade
            Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
            Sec-WebSocket-Protocol: chat
            */
            do
            {
               sq::http_response rsp;
               count = rsp.parser_http((const char *)msg, size);
               if (count <= 0)
               {
                   log_warn("parser_http error,msg={},count={}\n", (char *)msg,count);
                   break;
               }

               // rsp.dump();

               // 检查应答
               if (rsp.get_status_code() != 101)
               {
                   log_debug("shake fail code={},reason={}\n", rsp.get_status_code(),
                             rsp.get_status_reason());
                   break;
               }

               {
                   const char *val = rsp.get_head("upgrade");
                   if (val == nullptr)
                   {
                       log_debug("invalid upgrade\n");
                       break;
                   }

                   if (!compare_no_case(val, "websocket"))
                   {
                       log_debug("invalid upgrade\n");
                       break;
                   }
               }
               {
                   const char *val = rsp.get_head("sec-websocket-accept");

                   if (val == nullptr || compare_no_case(val, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") == false)
                   {
                       log_debug("invalid sec-websocket-accept,{}\n", val);
                       break;
                   }
               }
               log_debug("websocket client handshake ok\n");

               m_ws_state = ws_channel::ws_state_handshak_ok;
            } while (false);

            if(m_ws_state != ws_state_handshak_ok)
            {
                close();
                return size;
            }

            return count;
        }
        else{
            sq_panic("error state\n");
            return size;
        }
    }

    
}
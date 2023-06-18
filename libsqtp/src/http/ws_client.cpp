#include "http/ws_client.h"
#include "http/websocket.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "time/date_time.h"
#include "sq_macro.h"
#include "log/sq_logger.h"
#include "text/sq_string.h"
namespace sq
{

    ws_client::ws_client()
    {
        add_head("Upgrade", "websocket");
        add_head("Host", "127.0.0.1");
        add_head("Connection", "Upgrade");
        add_head("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ=="); //应该每次都不一样，随机生成
        add_head("Sec-WebSocket-Version", "13");
    }
    void ws_client::add_head(const char *key, const char *val)
    {
        m_heads[key] = val;
    }
    int ws_client::open(const char *server_ip, const char *url)
    {
        int ret = tcp_client::open(server_ip);
        if (ret!=ok)
        {
            return err_fail;
        }

        // handshake
        stringstream req;
        req << "GET " << url << " HTTP/1.1\r\n";

        auto it = m_heads.begin();
        //add head
        for (; it != m_heads.end(); ++it)
        {
            req << it->first << ": " << it->second << "\r\n";
        }
        req << "\r\n";
        shake_line = req.str();
        reg_on_msg(std::bind(&ws_client::on_message,
                                            this, std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3));
       
        //发送握手数据
        send_data(shake_line.c_str(), shake_line.size());
        
        
        return ok;
    }

    int ws_client::on_message(void *msg, int size, void *from)
    {

        if (m_state == 0) // 握手状态
        {
            /* rsp from server
            HTTP/1.1 101 Switching Protocols
            Upgrade: websocket
            Connection: Upgrade
            Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
            Sec-WebSocket-Protocol: chat

            */
            sq::http_response rsp;
            int ret=rsp.parser_http((const char *)msg, size);
            if(ret<0){
                close();
            }
            rsp.dump();
            // 检查应答
            if (rsp.get_status_code() != 101)
            {
                log_debug("shake fail code={},reason={}\n", rsp.get_status_code(),
                          rsp.get_status_reason());
                close();
                return size;
            }

            {
                const char*val=rsp.get_head("upgrade");
                if(val==nullptr){
                    log_debug("invalid upgrade\n");
                    close();
                    return size;
                }
                
                if (!compare_no_case(val,"websocket"))
                {
                    log_debug("invalid upgrade\n");
                    close();
                    return size;
                }
            }
            {
                const char*val=rsp.get_head("sec-websocket-accept");
                
                if (val==nullptr || compare_no_case(val,"s3pPLMBiTxaQ9kYGzzhZRbK+xOo=")==false)
                {
                    log_debug("invalid sec-websocket-accept,{}\n",val);
                    close();
                    return size;
                }
            }
            log_debug("websocket client handshake ok\n");

            m_state = 1;
            if (m_open_handler)
            {
                m_open_handler(this);
            }
            if (rsp.m_body_size > 0)
            {
                on_ws_message(rsp.m_body_begin, rsp.m_body_size);
            }
        }
        else if (m_state == 1)
        {
            on_ws_message((const char *)msg, size);
        }
        else
        {
            assert(false);
        }
        return size;
    }
    int ws_client::on_ws_message(const char*data,int size)
    {
        ws_package package=parser_websocket_msg((const unsigned char*)data,size);
        if(package.result!=ok){
            close();
            return ok;
        }
        //回调给应用层
        if(m_on_ws_msg){
            m_on_ws_msg(package.body,package.body_size);
        }
		return ok;
    }
    int ws_client::send_ws_package(uint8_t opcode, const char *data, uint32_t size, bool fin)
    {
        if(m_state!=1){
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
        bool ret=send_data(tmp_buf,pkg_size);
        if(ret){
            printf("send data ok ,body_size=%d\n",size);
        }
        return 0;
    }

    int ws_client::send_ws_message(const char *msg, int size)
    {
       
        return send_ws_package(OPCODE_TEXT,msg,size);
    }
}
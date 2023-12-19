#include "http/ws_client.h"
#include "http/websocket.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "time/date_time.h"
#include "net/net_address.h"
#include "sq_macro.h"
#include "log/sq_logger.h"
#include "text/sq_string.h"
namespace sq
{

    ws_client::ws_client(on_ws_channel_msg_fun_t on_msg,
			on_ws_channel_connected_fun_t on_connected ,
			on_ws_channel_disconnected_fun_t on_disconnected ,
			event_reactor *reactor)
    {
        m_on_ws_channel_msg = on_msg;
        m_on_ws_channel_connected = on_connected;
        m_on_ws_channel_disconnected = on_disconnected;
        if(reactor==nullptr)
        {
            m_reactor = get_default_reactor();
        }
        else{
            m_reactor = reactor;
        }
        
        
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
        m_ws_channel=add_ws_channel(server_ip);

        if (m_ws_channel==nullptr)
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
        string shake_line = req.str();
        m_ws_channel->reg_on_msg(std::bind(&ws_client::on_message,
                                            this, std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3));
       
        //发送握手请求
        m_ws_channel->send_data(shake_line.c_str(), shake_line.size());
        
        
        return ok;
    }
    void ws_client::close()
    {
        if(m_ws_channel){
            m_ws_channel->close();
            m_ws_channel=nullptr;
        }
    }
    ws_channel* ws_client::add_ws_channel(const char* server_address)
	{
		net_address address;
		address.parse_ipv4(server_address);
		if (address.m_port == 0){
			throw("Invalid port");
			return nullptr;
		}
			
		ws_channel*channel=new ws_channel(m_reactor);
		
		int ret = net_connect(channel->get_fd(), address.m_ip.c_str(), address.m_port, 5);

		if (ret == ok)
		{
			m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, channel);
			channel->state(handle_state::handle_opened);
            channel->m_ws_state=ws_channel::ws_state_handshaking;
		}
		else{
			delete channel;
			return nullptr;
		}

		return channel;
	}

    int ws_client::on_message(void *from,void *msg, int size)
    {
        int count=0;
        ws_channel* channel=static_cast<ws_channel*>(from);
        //客户端收到握手应答
        if (channel->m_ws_state == ws_channel::ws_state_handshaking) // 握手状态
        {
            count=channel->handshake_response((char*)msg,size);
            if(channel->m_ws_state==ws_channel::ws_state_handshak_ok)
            {
                if (m_on_ws_channel_connected)
                {
                    m_on_ws_channel_connected(channel);
                }
            }
            else{
                if (m_on_ws_channel_disconnected)
                {
                    m_on_ws_channel_disconnected(channel);
                }
            }
        }
        else if (channel->m_ws_state == ws_channel::ws_state_handshak_ok)
        {
            count= on_ws_message((ws_channel*)from,(const char *)msg, size);
        }
        else
        {
            assert(false);
        }
        return count;
    }
    int ws_client::on_ws_message(ws_channel*ch,const char*data,int size)
    {
        ws_package package=parser_websocket_msg((const unsigned char*)data,size);
        if(package.result!=ok){
            log_warn("parser websocket msg error,result={}\n",package.result);
            close();
            return 0;
        }
        //回调给应用层
        if(m_on_ws_channel_msg){
            m_on_ws_channel_msg(ch,package.body,package.body_size);
        }
		return package.pkg_size;
    }

    int ws_client::send_ws_message(const char *msg, int size)
    {
        if (m_ws_channel == nullptr)
        {
            printf("ws_channel is null\n");
            return err_fail;
        }
        return m_ws_channel->send_ws_package( msg, size,OPCODE_TEXT,true);
    }
}
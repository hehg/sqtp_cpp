#include "http/ws_server.h"
#include "net/net_address.h"
#include "log/sq_logger.h"
#include "http/ws_handler.h"
#include "http/websocket.h"
namespace sq
{
   
    ws_server::~ws_server()
    {
        auto it=m_sessions.begin();
        for(;it!=m_sessions.end();++it){
            it->second->close();
        }
        m_sessions.clear();
    }
    int ws_server::open(const  char* address)
	{
		m_fd = open_tcp_socket();
		unblock_socket(m_fd);
		
		net_address addr;
		addr.parse_ipv4(address);

		if (bind_socket(m_fd, addr.m_ip, addr.m_port) < 0)
		{
			perror("bind");
			throw "hx_net_listener:bind";
		}

		if (::listen(m_fd, 1024) < 0)
		{
			perror("listen");
			throw "tcp_server:listen";
		}
		event_handler::open();
		m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
		
		return ok;
	}
    void ws_server::on_read()
	{
		struct sockaddr_in it;
		socklen_t nameLen = sizeof(it);
		fd_t new_socket = ::accept(m_fd, (struct sockaddr *)&it, &nameLen);
		if (new_socket == invalid_fd)
		{
			return;
		}

		m_session_id++;

        ws_handler *h = new ws_handler(m_reactor, new_socket);
        h->reg_on_msg(std::bind(&ws_server::on_handshake_message, this, std::placeholders::_1,
                                std::placeholders::_2, std::placeholders::_3));
        h->reg_on_close(std::bind(&ws_server::on_session_close, this, std::placeholders::_1));
        h->m_session_id=m_session_id;
        h->m_state=ws_handler::ws_state_handshaking;
        m_sessions[m_session_id] = h;
       if(m_on_session_connected_fun){
            m_on_session_connected_fun(h);
        }
		
	}
    
    void ws_server::on_session_close(void *handler)
    {
        ws_handler *h=(ws_handler*)handler;
        m_sessions.erase(h->m_session_id);
        if(m_on_session_disconnected_fun){
            m_on_session_disconnected_fun(h);
        }
        h->m_session_id=0;
        h->m_state=ws_handler::ws_state_closed;
    }
    //来自客户端的消息
    int ws_server::on_handshake_message(void *msg, int size, void *handler)
    {
        ws_handler *h = (ws_handler *)handler;
        if (h->m_state == ws_handler::ws_state_handshaking)
        {
            int ret= h->handshake_request((char *)msg, size);
            if(ret!=ok){
                log_info("hand shake fail,msg={}\n",(char*)msg);
                ws_handler*h=(ws_handler*)handler;
                h->close();
            }

            if(m_on_ws_handshake_ok_fun){
                m_on_ws_handshake_ok_fun(h);
            }
            h->m_state=ws_handler::ws_state_handshak_ok;
            return size;
        }
        else if(h->m_state==ws_handler::ws_state_handshak_ok) //握手结束
        {
            //包还没收全 
            ws_package pack=parser_websocket_msg((const unsigned char*)msg,size);
            if(pack.result==err_again){
                return 0;
            }
            if(pack.result!=ok){
                log_info("invalid ws message,ret={}\n",pack.result);
                h->close();
            }

            if(m_on_ws_msg_fun){
                m_on_ws_msg_fun(pack.body,pack.body_size,handler);
            }
            
            return pack.pkg_size;

        }
        else{
            return size;        
        }
       
        
    }

    void  ws_server::send_msg(void*handler,const char*msg,int size)
    {
        ws_handler *h=(ws_handler*)handler;
        if(h->m_state==ws_handler::ws_state_handshak_ok){
            int len=make_ws_package(msg,size,tmp_buf,sizeof(tmp_buf),0);
            h->send_data(tmp_buf,len);
        }
    }
    
}
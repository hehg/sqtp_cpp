#include "net/tcp_client.h"
#include "net/reactor.h"
#include "sq_macro.h"
#include "net/tcp_channel.h"
#include "net/net_address.h"
#include "net/tcp_channel.h"
namespace sq
{

	tcp_client::tcp_client(on_tcp_channel_msg_fun_t on_msg,
		on_tcp_channel_connected_fun_t on_connected,
		on_tcp_channel_disconnected_fun_t on_disconnected,
		event_reactor *r)
	{
		m_on_connected=on_connected;
		m_on_disconnected=on_disconnected;
		m_on_msg=on_msg;
		if (r)
		{
			m_eg = r;
		}
		else{
			m_eg=get_default_reactor();
		}
	}

	tcp_client::~tcp_client()
	{
      
	}
	void tcp_client::close()
	{
		for(auto it=m_channels.begin();it!=m_channels.end();++it)
		{
			it->second->close();
		}
		m_channels.clear();
	}
    int tcp_client::send(const char*data,int size)
	{
		if(m_channels.empty()){
			return 0;
		}
		
       auto it=m_channels.begin();
	   for (;it!=m_channels.end();++it)	
	   {
			it->second->send_data(data,size);
	   }
	   
	  return ok;
	}

	int tcp_client::add_channel(const char* server_address)
	{
		net_address address;
		address.parse_ipv4(server_address);
		if (address.m_port == 0){
			throw("Invalid port");
			return err_invalid_param;
		}
			
		tcp_channel*channel=new tcp_channel(m_eg);
		channel->reg_on_open(std::bind(&tcp_client::on_channel_connected, this, std::placeholders::_1));
		channel->reg_on_close(std::bind(&tcp_client::on_channel_disconnected, this, std::placeholders::_1));
		channel->reg_on_msg(std::bind(&tcp_client::on_channel_message, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));
		int ret = net_connect(channel->get_fd(), address.m_ip.c_str(), address.m_port, 5);

		if (ret == ok)
		{
			m_channels[channel->get_fd()] = channel;
			m_eg->post_msg(EVENT_MSG_ADD_HANDLER, channel);
			channel->state(handle_state::handle_opened);
		}
		else{
			delete channel;
			return err_fail;
		}

		return ok;
	}

	 int tcp_client::on_channel_message(void*channel,void*package, int size)
	 {
		if(m_on_msg){
			return m_on_msg((tcp_channel*)channel,package,size);
		}
		return size;
	 }
	 void tcp_client::on_channel_connected(void*ch)
	 {
		if(m_on_connected){
			m_on_connected((tcp_channel*)ch);
		}
	 }
	 void tcp_client::on_channel_disconnected(void*ch)
	 {
		tcp_channel*tcp_ch=(tcp_channel*)ch;
		if(m_on_disconnected){
			m_on_disconnected(tcp_ch);
		}
		m_channels.erase(tcp_ch->get_fd());
	 }
}
#include "net/tcp_channel.h"
#include "net/reactor.h"
#include "sq_macro.h"
#include "time/date_time.h"
namespace sq{
tcp_channel::tcp_channel():event_handler()
	{
		create();
		//m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
	}
	tcp_channel::tcp_channel(event_reactor*r):event_handler(r)
	{
		create();
		//m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
	}
	tcp_channel::tcp_channel(event_reactor*r, fd_t fd) 
		:event_handler(r), m_fd(fd) {
		m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
		assert(fd > 0);
		state(handle_state::handle_opening);
	}
	tcp_channel::~tcp_channel()
	{
		if (m_fd != invalid_fd) {
			close_socket(m_fd);
			m_fd = invalid_fd;
		}
	}
	void tcp_channel::close()
	{
		if (m_fd != invalid_fd) {
			close_socket(m_fd);
			m_fd = invalid_fd;
		}
		base::close();
	}
	void tcp_channel::create()
	{
		if(m_fd!=invalid_fd){
			close_socket(m_fd);
			m_fd=invalid_fd;
		}
		m_fd = open_tcp_socket();
		unblock_socket(m_fd);
		net_set_send_buffer_size(m_fd, 40960);
		nodelay_tcp_socket(m_fd);
	}
	int tcp_channel::send_data(const char*buf,int len)
	{
		//std::lock_guard<std::mutex> lk(m_write_mtx);

		if (m_state!=handle_state::handle_opened) {
			printf("disconnect can not send_data\n");
			return err_net_connect;
		}
		int sended = 0;
		do {
			int flag = 0;
#ifdef LINUX
			flag = MSG_NOSIGNAL;
#endif
			int write = send(m_fd, buf + sended, len - sended, flag);
			if (write < 0)
			{
				int err = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
                if (WSAEWOULDBLOCK == err || WSATRY_AGAIN == err){
#else
                if (err == EWOULDBLOCK || err == EBUSY || err == 0 || err == ENOENT || err == ENOSYS || err == EINPROGRESS) {
#endif
					continue;
				}
				else
				{
					perror("send");
					close();
					return err_net_connect;
				}
			}
			sended += write;
		} while (sended < len);

		return ok;
	}
	void tcp_channel::on_read()
	{
        int64_t buf_len = 1024;

		char *buf = m_net_cahce.write_space(buf_len);
		if (buf)
		{
			int read_len = recv(m_fd, buf, buf_len, 0);
			
			if (read_len == -1)
			{
                int err = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
                if (WSAEWOULDBLOCK == err || WSATRY_AGAIN == err) {
#else
                if (err == EWOULDBLOCK || err == EBUSY || err == 0 || err == ENOENT || err == ENOSYS || err == EINPROGRESS) {
#endif
                    return;
                }
                else {
					close();
                }
				
			}
			else if (read_len == 0) {
				close();
			}
			m_net_cahce.push_back(read_len);

		}
		dispatch();
	}
	void tcp_channel::dispatch()
	{
        int64_t can_read_size = 0;
		while (true)
		{
			can_read_size = 0;
			char*msg = m_net_cahce.read_space(can_read_size);
			if (msg == nullptr|| can_read_size==0)
				break;
			int ret=on_message(msg, can_read_size,this);
			if (ret <= 0)
				break;
			assert(ret <= can_read_size);
			m_net_cahce.pop_front(ret);

		}
	}

	void tcp_channel::get_peer_addr(string &peer_addr)
	{
		if (m_fd != invalid_fd)
		{
			get_peer_ip_address(m_fd, peer_addr);
		}
	}
} 
	


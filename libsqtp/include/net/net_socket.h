#pragma once
#include "sq_define.h"
#include "sq_types.h"
#include "net_init.hpp"
#if (defined MAC)

struct mmsghdr
{
	struct msghdr msg_hdr;
	unsigned int msg_len;
};

#endif

#if ( defined WINDOWS)
struct iovec
{
	void *iov_base;
	size_t iov_len;
};
struct msghdr
{
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	size_t msg_iovlen;
	void *msg_control;
	size_t msg_controllen;
	int msg_flags;
};
struct mmsghdr
{
	struct msghdr msg_hdr;
	unsigned int msg_len;
};
#define MSG_DONTWAIT 1
#define MSG_NOSIGNAL 2
#endif
namespace sq
{
	fd_t open_socket(int domain_, int type_, int protocol_);
	inline fd_t open_tcp_socket() { return open_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); }
	inline fd_t open_udp_socket() { return open_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); }

	void close_socket(fd_t);
	
	int close_wait_ms(int fd_, unsigned int max_ms_ = 2000);

	int bind_socket(fd_t s, string ip, int port);

	void unblock_socket(fd_t s, bool flag = true);

	bool get_peer_ip_address(fd_t sockfd_, std::string &ip_addr_);

	void get_local_ip(std::map<string, string> &ips);

	//  Tunes the supplied TCP socket for the best latency.
	void nodelay_tcp_socket(fd_t s, int flag = 1);

	//  Sets the socket send buffer size.
	void net_set_send_buffer_size(fd_t sockfd_, int bufsize_);

	//  Sets the socket receive buffer size.
	void net_set_recv_buffer_size(fd_t sockfd_, int bufsize_);

	//延迟关闭
	void net_lingering_close(fd_t sockfd_, int timeout);
	
	//组播相关
	void net_join_to_group(fd_t socket, string group_ip, string my_ip, int group_port);

	void net_leave_group(fd_t socket, string group_ip, string my_ip, int group_port);

	int enable_multi_cast_loop(fd_t socket, bool enable);

#if (defined WINDOWS || defined MAC)
	int socketpair(int domain, int type, int protocol, int sv[2], int signaler_port = 65530);

	int32_t recvmsg(fd_t socket, struct msghdr *msg, unsigned int flags);
	int recvmmsg(fd_t s, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags, struct timespec *timeout);
#endif
	bool is_real_socket_error();

	int net_send_udp_msg(fd_t socket,const char*msg,int len,const char*ip,int port);

    void net_sockaddr_to_string(sockaddr*addr,string&ip,int&port);
	/** 建立tcp连接
	 * @param timeout 超时时间 单位second
	*/
	int net_connect(fd_t socket,const char*ip,int port,int timeout);
	
	//查询内核获取最后一个数据包的时间
	int64_t get_package_recv_time_by_core(fd_t socket);
	//=========================================================
	/** @class udp_socket
	* @brief  单播和多播的基类
	*
	*  实现最基本功能的封装
	*/
	class udp_socket
	{
	public:
		udp_socket();
		udp_socket(const char *url, bool is_sender);

		~udp_socket();
        /**
		 * @param  url  地址
		 *         udp://127.0.0.1:4567   单播
		 *         ump://232.0.10.1:4567  组播形式
		 * @param  is_sender 是否为发送端
		 * @param  local_ip 本机ip
		 */
		void open(const char *url, bool is_sender,const char* local_ip=nullptr);

		/// 设置socket为非阻塞方式
		int32_t set_unblock();

		/// 接收数据
		int32_t recv_from(char *buf, int32_t buf_len);

		void set_send_buf_size(int size);
		void set_recv_buf_size(int size);

		/// 发送数据包到已经设置好的对端地址和端口
		int32_t send_to_dest(const char *buf, int32_t buf_len) const;

		// 设置TTL
		int32_t set_multicast_ttl(int32_t ttl);

		// 开启本地LOOP，是否交付到本地计算机的组播监听端口，默认为开启
		int32_t enable_multilcast_loop(bool enable);

		// 设置发送数据时使用的本地接口(IP)
		int32_t set_multicast_if(const char *local_ip);

	protected:
		bool m_is_sender = false;
		struct sockaddr_in m_dest_addr;	  ///对端地址结构体
		struct ip_mreq m_mreq;			  ///组播会话结构体，在设置socket属性时使用
		struct ip_mreq_source m_mreq_src; ///指定源的多播地址
	public:
		fd_t m_socket_fd = invalid_fd; ///SOCKET描述符
		char  m_from_addr[128];
		sockaddr m_from;
	};

} // namespace sq

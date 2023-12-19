#include "net/net_socket.h"
#include "sq_define.h"
#include "sq_macro.h"
#include "error/sq_err.h"
#include "net/net_address.h"
#include "log/sq_logger.h"
#include "error/sq_err.h"

#if defined(LINUX)
#define HAVE_SOCK_CLOEXEC
#endif
namespace sq
{

	fd_t open_socket(int domain_, int type_, int protocol_)
	{
		//  Setting this option result in sane behaviour when exec() functions
		//  are used. Old sockets are closed and don't block TCP ports etc.
#if defined HAVE_SOCK_CLOEXEC
		type_ |= SOCK_CLOEXEC;
#endif

		fd_t s = socket(domain_, type_, protocol_);
		if (s == -1)
			return -1;
		int on_port = 1;
#if ( defined(linux))
		setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (char *)&on_port, sizeof(int));
#endif
		int on = 1;
		int ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

		//  If there's no SOCK_CLOEXEC, let's try the second best option. Note that
		//  race condition can cause socket not to be closed (if fork happens
		//  between socket creation and this point).
#if !defined HAVE_SOCK_CLOEXEC && defined FD_CLOEXEC
		int rc = fcntl(s, F_SETFD, FD_CLOEXEC);
		assert(rc != -1);
#endif

		//  On Windows, preventing sockets to be inherited by child processes.
#if defined(WINDOWS)
		BOOL brc = SetHandleInformation((HANDLE)s, HANDLE_FLAG_INHERIT, 0);
		assert(brc);
#endif

		return s;
	}
	int close_wait_ms(int fd_, unsigned int max_ms_)
	{
		unsigned int ms_so_far = 0;
		unsigned int step_ms = max_ms_ / 10;
		if (step_ms < 1)
			step_ms = 1;

		if (step_ms > 100)
			step_ms = 100;

		int rc = 0; // do not sleep on first attempt

		do
		{
			if (rc == -1 && errno == EAGAIN)
			{
				// sq_util::sleep_ms(step_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));
				ms_so_far += step_ms;
			}

			rc = close(fd_);
		} while (ms_so_far < max_ms_ && rc == -1 && errno == EAGAIN);

		return rc;
	}
	void close_socket(fd_t s)
	{

#if defined WINDOWS
		closesocket(s);
#else
		close(s);
#endif
	}

	int bind_socket(fd_t s, string ip, int port)
	{
		sockaddr sock_addr;
		memset(&sock_addr, 0, sizeof(struct sockaddr));
		sockaddr_in *addr_in = (sockaddr_in *)&sock_addr;

		if (!ip.empty() && ip != "0.0.0.0")
		{
			addr_in->sin_addr.s_addr = inet_addr(ip.c_str());
		}
		else
		{
			addr_in->sin_addr.s_addr = htonl(INADDR_ANY);
		}
		addr_in->sin_family = AF_INET;
		addr_in->sin_port = htons(port);
		int ret=::bind(s, &sock_addr, sizeof(sock_addr));
		if(ret< 0)
		{
			sq_panic("bind socket failed,fd=%d,ret=%d,msg=%s\n",s,ret,get_last_error_info());
			return -1;
		}
		return 0;
	}
	void unblock_socket(fd_t s_, bool flag)
	{
#if defined(WINDOWS)
		u_long nonblock = 1;
		if (flag == false)
			nonblock = 0;

		int rc = ioctlsocket(s_, FIONBIO, &nonblock);
		if (rc == -1)
		{
			get_last_error_info();
			perror("unblock_socket");
			assert(rc != SOCKET_ERROR);
		}

#else
		int flags = fcntl(s_, F_GETFL, 0);
		if (flags == -1)
			flags = 0;
		int rc;
		if (flag == true)
			rc = fcntl(s_, F_SETFL, flags | O_NONBLOCK);
		else
			rc = fcntl(s_, F_SETFL, flags & (~O_NONBLOCK));
#endif
	}

	bool get_peer_ip_address(fd_t sockfd_, std::string &ip_addr_)
	{
		int rc;
		struct sockaddr_in ss;

#if defined(WINDOWS)
		int addrlen = static_cast<int>(sizeof ss);
#else
		socklen_t addrlen = sizeof ss;
#endif
		rc = getpeername(sockfd_, (struct sockaddr *)&ss, &addrlen);
#if defined(WINDOWS)
		if (rc == SOCKET_ERROR)
		{
			assert(WSAGetLastError() != WSANOTINITIALISED &&
				   WSAGetLastError() != WSAEFAULT &&
				   WSAGetLastError() != WSAEINPROGRESS &&
				   WSAGetLastError() != WSAENOTSOCK);
			return false;
		}
#else
		if (rc == -1)
		{
			assert(errno != EBADF &&
				   errno != EFAULT &&
				   errno != EINVAL &&
				   errno != ENOTSOCK);
			return false;
		}
#endif
		char host[128];
		inet_ntop(AF_INET, &ss.sin_addr, host, sizeof(host));
		int port = ntohs(ss.sin_port);

		ip_addr_ = host;
		return true;
	}

	void get_local_ip(std::map<string, string> &ips)
	{
#ifdef linux
		struct ifaddrs *ifas = NULL;
		struct ifaddrs *ifa = NULL;
		void *tmpAddrPtr = NULL;

		getifaddrs(&ifas);

		for (ifa = ifas; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_addr->sa_family == AF_INET)
			{
				// is a valid IP4 Address
				tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
				if (strcmp("127.0.0.1", addressBuffer) == 0)
				{
					continue;
				}
				string key(ifa->ifa_name);
				string value(addressBuffer);
				ips.insert(std::pair<string, string>(key, value));

				// printf("'%s': %s\n", ifa->ifa_name, addressBuffer);
			}
		}
		if (ifas != NULL)
			freeifaddrs(ifas);
#endif
	}
	void nodelay_tcp_socket(fd_t s_, int flag)
	{
		//  Disable Nagle's algorithm. We are doing data batching on 0MQ level,
		//  so using Nagle wouldn't improve throughput in anyway, but it would
		//  hurt latency.
		int nodelay = flag;
		int rc = setsockopt(s_, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay,
							sizeof(int));
#if defined(WINDOWS)
		assert(rc != SOCKET_ERROR);
#else
		assert(rc == 0);
#endif
	}

	void net_set_send_buffer_size(fd_t sockfd_, int bufsize_)
	{
		const int rc = setsockopt(sockfd_, SOL_SOCKET, SO_SNDBUF,
								  (char *)&bufsize_, sizeof bufsize_);
		if (rc != 0)
		{
			log_warn("net_set_send_buffer_size err,ret={},msg={}\n",rc,get_last_error_info());
		}
	}

	void net_set_recv_buffer_size(fd_t sockfd_, int bufsize_)
	{
		const int rc = setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF,
								  (char *)&bufsize_, sizeof bufsize_);
		if (rc != 0)
		{
			log_warn("net_set_recv_buffer_size err,ret={},msg={}\n",rc,get_last_error_info());
		}
	}
	void net_lingering_close(fd_t sockfd_, int timeout)
	{
		struct linger so_linger;
		so_linger.l_onoff = 1;
		so_linger.l_linger = timeout;
		int rc = setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF,
								 (const char*) &so_linger, sizeof so_linger);
#if defined(WINDOWS)
		assert(rc != SOCKET_ERROR);
#else
		assert(rc == 0);
#endif
	}
	void net_join_to_group(fd_t socket, string group_ip, string my_ip, int group_port)
	{
		assert(group_ip.length() > 0);
		assert(group_port > 0);

		struct ip_mreq req;

		if (my_ip.length() > 0)
			req.imr_interface.s_addr = inet_addr(my_ip.c_str());
		else
			req.imr_interface.s_addr = htonl(INADDR_ANY);
		req.imr_multiaddr.s_addr = inet_addr(group_ip.c_str());

		int ret = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&req, sizeof(req));
		if (ret < 0)
		{
			perror("setsockopt");
		}
	}
	void net_leave_group(fd_t socket, string group_ip, string my_ip, int group_port)
	{
		assert(group_ip.length() > 0);
		assert(group_port > 0);

		struct ip_mreq req;

		req.imr_interface.s_addr = inet_addr(my_ip.c_str());
		req.imr_multiaddr.s_addr = inet_addr(group_ip.c_str());

		int ret = setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char *)&req, sizeof(req));
		if (ret < 0)
		{
			assert(false);
		}
	}
	// 开启本地LOOP
	int enable_multi_cast_loop(fd_t socket, bool enable)
	{
		int c = 1;
		if (enable == false)
			c = 0;
		return setsockopt(socket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&c, sizeof(c));
	}
#if (defined WINDOWS || defined __APPLE__)
	int32_t recvmsg(fd_t socket, struct msghdr *msg, unsigned int flags)
	{
		int32_t recv_len = 0;
		if (!msg || !msg->msg_iov || !msg->msg_iov[0].iov_base)
		{
			return recv_len;
		}
		if (flags & MSG_DONTWAIT)
		{
			unblock_socket(socket);
		}
		else
		{
			unblock_socket(socket, false);
		}
		int rs = 0;
		if (msg->msg_name != nullptr)
		{
			rs = recvfrom(socket, (char *)(msg->msg_iov[0].iov_base), msg->msg_iov[0].iov_len, 0,
						  (sockaddr *)msg->msg_name, &msg->msg_namelen);
		}
		else
		{
			rs = recv(socket, (char *)(msg->msg_iov[0].iov_base), msg->msg_iov[0].iov_len, 0);
		}
		if (rs < 0)
			return rs;
		recv_len = rs;
		return recv_len;
	}

	int32_t sendmsg(fd_t s, const struct msghdr *msg, unsigned int flags)
	{
		if (!msg)
		{
			return 0;
		}
		int32_t send_len = 0;
		for (int i = 0; i < msg->msg_iovlen; ++i)
		{
			int ret = sendto(s, (char *)msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len, flags, (const sockaddr *)msg->msg_name,
							 msg->msg_namelen);
			if (ret == 0)
			{
				return send_len;
			}
			else if (ret == -1)
			{
				printf("sendto err,ret=%d,msg=%s\n",ret,get_last_error_info());
				return -1;
			}
			else
			{
				send_len += ret;
			}
		}
		return send_len;
	}
#endif
#if (defined WINDOWS || defined __APPLE__)
	int sendmmsg(fd_t s, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags)
	{
		int count = 0;
		for (unsigned int i = 0; i < vlen; i++)
		{
			int size = sendmsg(s, &(msgvec + i)->msg_hdr, flags);
			if (size == -1)
			{
				return 0 == i ? -1 : i;
			}
			msgvec->msg_len += size;
			count++;
		}
		return count;
	}
	int recvmmsg(fd_t s, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags, struct timespec *timeout)
	{
		int count = 0;
		for (unsigned int i = 0; i < vlen; i++)
		{
			(msgvec + i)->msg_len = 0;
			int size = recvmsg(s, &(msgvec + i)->msg_hdr, flags);
			if (-1 == size)
			{
				return 0 == i ? -1 : i;
			}
			else if (size == 0)
			{
				break;
			}
			(msgvec + i)->msg_len += size;
			count++;
		}
		return count;
	}
#endif
	void net_sockaddr_to_string(sockaddr *addr, string &ip, int &port)
	{
		sockaddr_in *pin = (sockaddr_in *)(addr);
		const char *from_addr = inet_ntoa(pin->sin_addr);
		port = ntohs(pin->sin_port);
		ip = from_addr;
	}

	///==============udp_socket============================================

	udp_socket::udp_socket()
	{
	}
	udp_socket::udp_socket(const char *url, bool is_sender)
	{
		open(url, is_sender);
	}

	void udp_socket::open(const char *url, bool is_sender, const char *local_ip)
	{
		m_socket_fd = open_udp_socket();
		memset(&m_dest_addr, 0, sizeof(m_dest_addr));

		m_is_sender = is_sender;
		sq::net_address ip;
		string my_url = url;
		bool ret=ip.parse_ipv4(my_url);
		assert(ret);

		if (m_is_sender)
		{
			m_dest_addr.sin_family = AF_INET;
			m_dest_addr.sin_addr.s_addr = inet_addr(ip.m_ip.c_str());
			m_dest_addr.sin_port = htons(ip.m_port);
		}
		else
		{
			if (ip.m_is_multicast)
			{
				bind_socket(m_socket_fd, "", ip.m_port);
				string my_ip = "";
				if (local_ip)
				{
					my_ip = local_ip;
				}
				net_join_to_group(m_socket_fd, ip.m_ip, my_ip, ip.m_port);
			}
			else
			{
				bind_socket(m_socket_fd, ip.m_ip, ip.m_port);
			}
		}
	}

	/**
	 * @brief		UDPSocket析构函数
	 * @return		无
	 */
	udp_socket::~udp_socket()
	{
		// net_leave_group(m_socket_fd, m_dest_ip, m_local_ip, m_dest_port);
		close_socket(m_socket_fd);
	}

	

	int32_t udp_socket::send_to_dest(const char *buf, int32_t buf_len) const
	{
		int32_t repeat = 0;
		int32_t ret = -1;

		do
		{
			ret = sendto(m_socket_fd, (const char *)buf, buf_len, 0, (sockaddr *)&m_dest_addr, sizeof(struct sockaddr));

			if (++repeat > 100)
			{
				perror("udp_socket::send_to_dest error");
				return ret;
			}
			if (ret == -1)
			{
				int32_t err_code = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
				if (WSAEWOULDBLOCK == err_code || WSATRY_AGAIN == err_code)
#else
				if (EWOULDBLOCK == err_code || EAGAIN == err_code)
#endif

				{
					continue;
				}
				else
				{
					printf("sendto error,ret=%d,msg=%s\n",ret,get_last_error_info());
					return -1;
				}
			}

		} while (ret < 0);

		return ret;
	}

	int32_t udp_socket::recv_from(char *buf, int32_t max_len)
	{
		socklen_t from_len = sizeof(sockaddr);

		int32_t ret = recvfrom(m_socket_fd, buf, max_len, 0, &m_from, &from_len);
#if 0
	sockaddr_in *pin = (sockaddr_in *)(&m_from);
	const char* from_addr=inet_ntoa(pin->sin_addr);
	int port=ntohs(pin->sin_port);
	sprintf(m_from_addr,"%s:%d\n",from_addr,port);
#endif

		if (ret == -1)
		{
			int32_t err_code = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
			if (WSAEWOULDBLOCK == err_code || WSATRY_AGAIN == err_code)
#else
			if (EWOULDBLOCK == err_code || EAGAIN == err_code)
#endif
			{
				return 0;
			}

			else if (EMSGSIZE == GET_LAST_SOCK_ERROR())
			{
				perror("receive buffer is too small.\n");
			}
			else
			{
				// perror("mudp_socket recv error");
			}
		}
		return ret;
	}

	// 设置TTL
	int32_t udp_socket::set_multicast_ttl(int32_t ttl)
	{
		return setsockopt(m_socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	}

	// 开启本地LOOP
	int32_t udp_socket::enable_multilcast_loop(bool enable)
	{
		int32_t loopback = (enable ? 1 : 0);
		return setsockopt(m_socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopback, sizeof(loopback));
	}

	int32_t udp_socket::set_multicast_if(const char *szLocalIP)
	{
		if (nullptr != szLocalIP)
		{
			struct in_addr objAddr;
			objAddr.s_addr = inet_addr(szLocalIP);
#if (defined WINDOWS || defined __APPLE__)
			return setsockopt(m_socket_fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&objAddr, sizeof(in_addr));
#else
			return setsockopt(m_socket_fd, SOL_IP, IP_MULTICAST_IF, &objAddr, sizeof(in_addr));
#endif
		}

		return -1;
	}

#ifdef WINDOWS
	int socketpair(int domain, int type, int protocol, int sv[2], int signaler_port)
	{
		SECURITY_DESCRIPTOR sd;
		SECURITY_ATTRIBUTES sa;
		memset(&sd, 0, sizeof(sd));
		memset(&sa, 0, sizeof(sa));

		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = &sd;

		HANDLE sync = CreateEvent(&sa, FALSE, TRUE, TEXT("Global\\sq-socketpair-port-sync"));

		if (sync == NULL && GetLastError() == ERROR_ACCESS_DENIED)
			sync = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE,
							 FALSE, TEXT("Global\\sq-signaler-port-sync"));

		assert(sync != NULL);

		//  Windows has no 'socketpair' function. CreatePipe is no good as pipe
		//  handles cannot be polled on. Here we create the socketpair by hand.
		sv[0] = INVALID_SOCKET;
		sv[1] = INVALID_SOCKET;

		//  Create listening socket.
		SOCKET listener;
		listener = open_socket(AF_INET, SOCK_STREAM, 0);
		assert(listener != INVALID_SOCKET);

		//  Set SO_REUSEADDR and TCP_NODELAY on listening socket.
		BOOL so_reuseaddr = 1;
		int rc = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
							(char *)&so_reuseaddr, sizeof(so_reuseaddr));
		assert(rc != SOCKET_ERROR);
		BOOL tcp_nodelay = 1;
		rc = setsockopt(listener, IPPROTO_TCP, TCP_NODELAY,
						(char *)&tcp_nodelay, sizeof(tcp_nodelay));
		assert(rc != SOCKET_ERROR);

		//  Init sockaddr to signaler port.
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(signaler_port);

		//  Create the writer socket.
		sv[1] = open_socket(AF_INET, SOCK_STREAM, 0);
		assert(sv[1] != INVALID_SOCKET);

		//  Set TCP_NODELAY on writer socket.
		rc = setsockopt(sv[1], IPPROTO_TCP, TCP_NODELAY,
						(char *)&tcp_nodelay, sizeof(tcp_nodelay));
		assert(rc != SOCKET_ERROR);

		//  Enter the critical section.
		DWORD dwrc = WaitForSingleObject(sync, INFINITE);
		assert(dwrc == WAIT_OBJECT_0);

		//  Bind listening socket to signaler port.
		rc = ::bind(listener, (const struct sockaddr *)&addr, sizeof(addr));

		//  Listen for incoming connections.
		if (rc != SOCKET_ERROR)
			rc = listen(listener, 1);

		//  Connect writer to the listener.
		if (rc != SOCKET_ERROR)
			rc = connect(sv[1], (struct sockaddr *)&addr, sizeof(addr));

		//  Accept connection from writer.
		if (rc != SOCKET_ERROR)
			sv[0] = accept(listener, NULL, NULL);

		//  Save errno if error occurred in bind/listen/connect/accept.
		int saved_errno = 0;
		if (sv[0] == INVALID_SOCKET)
			saved_errno = WSAGetLastError();

		//  We don't need the listening socket anymore. Close it.
		closesocket(listener);

		//  Exit the critical section.
		BOOL brc = SetEvent(sync);
		assert(brc != 0);

		//  Release the kernel object
		brc = CloseHandle(sync);
		assert(brc != 0);

		if (sv[0] != INVALID_SOCKET)
		{

			//  On Windows, preventing sockets to be inherited by child processes.
			brc = SetHandleInformation((HANDLE)sv[0], HANDLE_FLAG_INHERIT, 0);
			assert(brc);

			return 0;
		}
		else
		{
			//  Cleanup writer if connection failed
			if (sv[1] != INVALID_SOCKET)
			{
				rc = closesocket(sv[1]);
				assert(rc != SOCKET_ERROR);
				sv[1] = INVALID_SOCKET;
			}
			//  Set errno from saved value
			errno = saved_errno;
			return -1;
		}

		return 0;
	}
#endif
	bool is_real_socket_error()
	{
		int32_t err_code = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
		if (WSAEWOULDBLOCK == err_code || WSATRY_AGAIN == err_code)
#else
		if (EWOULDBLOCK == err_code || EAGAIN == err_code)
#endif
		{
			return false;
		}
		return true;
	}
	int net_send_udp_msg(fd_t socket, const char *msg, int len, const char *ip, int port)
	{
		sockaddr_in dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_addr.s_addr = inet_addr(ip);
		dest_addr.sin_port = htons(port);

		int32_t repeat = 0;
		int32_t ret = -1;

		do
		{
			ret = sendto(socket, (const char *)msg, len, 0, (sockaddr *)&dest_addr, sizeof(struct sockaddr));

			if (++repeat > 100)
			{
				perror("udp_socket::send_to_dest error");
				return err_net_connect;
			}
			if (ret == -1)
			{
				int32_t err_code = GET_LAST_SOCK_ERROR();
#ifdef WINDOWS
				if (WSAEWOULDBLOCK == err_code || WSATRY_AGAIN == err_code)
#else
				if (EWOULDBLOCK == err_code || EAGAIN == err_code)
#endif

				{
					continue;
				}
				else
				{
					printf("sendto error,ret=%d,msg=%s\n",ret,get_last_error_info());
					return err_net_connect;
				}
			}

		} while (ret < 0);

		return ok;
	}

	int net_connect(fd_t fd, const char *ip, int port, int timeout)
	{
		if (fd < 0)
		{
			return err_invalid_package;
		}
		struct sockaddr_in tcp_server_addr;
		memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
		tcp_server_addr.sin_family = AF_INET;
		tcp_server_addr.sin_addr.s_addr = inet_addr(ip);
		tcp_server_addr.sin_port = htons(port);

		// inet_aton(ip, &tcp_server_addr.sin_addr);
		bool connect_ok = false;
		int ret = ::connect(fd, (struct sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr));
		if (ret >= 0)
		{
			connect_ok = true;
		}
		else
		{
			// 异步socket  继续检查，等待3次握手结束
			if (errno == EINPROGRESS || errno == EALREADY)
			{
				time_t begin = time(nullptr);
				for (;;)
				{
					// check is timeout
					time_t end = time(nullptr);
					if (end - begin > timeout)
					{
						return err_time_out;
					}

					fd_set writefds;
					struct timeval timeout;

					FD_ZERO(&writefds);
					FD_SET(fd, &writefds);
					timeout.tv_sec = 1; // wait 1s
					timeout.tv_usec = 0;
					int ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
					if (ret == 0)
					{
						continue;
					}

					struct sockaddr name;
					if ((ret > 0) && FD_ISSET(fd, &writefds))
					{
						socklen_t nameLen = sizeof(name);
						if (getpeername(fd, &name, &nameLen) == 0)
						{
							connect_ok = true;
							break;
						}
					}
				}
			}
		}

		if (connect_ok)
		{
			return ok;
		}
		else
		{
			return err_fail;
		}
	}
	int64_t get_package_recv_time_by_core(fd_t socket)
	{
#if (defined LINUX)
		timeval tv;
		int ret = ioctl(socket, SIOCGSTAMP, &tv);

		if (ret == 0)
		{
			uint64_t d = tv.tv_sec * 1000000 + tv.tv_usec;
			return d;
		}
#endif
		return 0;
	}
} // namespace sq

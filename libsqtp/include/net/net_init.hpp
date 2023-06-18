#pragma once
#include "sq_define.h"
#include "sync/sq_atomic.h"
namespace sq
{
	static atomic_counter init_counter;
	class net_init
	{
	public:
		net_init()
		{
#ifdef WINDOWS
			if (init_counter == 0)
			{
				// init Windows socket
				WORD wVersionRequested;
				WSADATA wsaData;
				wVersionRequested = MAKEWORD(2, 2);
				int ret = WSAStartup(wVersionRequested, &wsaData);
				if (0 != ret)
				{
					perror("WSAStartup()");

				}
				init_counter++;
			}
			
#endif
		}
		~net_init()
		{
#ifdef WINDOWS
			WSACleanup();
#endif
		}
	};

	static const net_init net_instance = net_init();

}
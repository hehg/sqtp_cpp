/**
Copyright (c) 2003-2015 
@brief 预定义


@author 
*/
#pragma once
//检测编译器版本
#if defined(WINDOWS)
//#if defined(_MSC_VER) && (_MSC_VER >= 1900)
//#error "not support compiler,please use vs2015 or above"
    #define SUPPORT_SELECT 1
    #define SUPPORT_EPOLL  0
    #define SUPPORT_EVENT_FD 0
    #define SUPPORT_TIMER_FD 0
#endif
#if defined (__linux__)
    #define SUPPORT_SELECT 0
    #define SUPPORT_EPOLL  1
    #define SUPPORT_EVENT_FD 1
    #define SUPPORT_TIMER_FD 1
#endif
#if defined (__APPLE__)
    #define SUPPORT_SELECT 1
    #define SUPPORT_EPOLL  0
    #define SUPPORT_EVENT_FD 0
    #define SUPPORT_TIMER_FD 0
#endif


// #if defined(SUPPORT_EPOLL)
// #define SUPPORT_SELECT 0
// #define SUPPORT_EPOLL  1
// #define SUPPORT_EVENT_FD 0
// #define SUPPORT_TIMER_FD 1
// #else
// #define SUPPORT_SELECT 1
// #define SUPPORT_EPOLL  0
// #define SUPPORT_EVENT_FD 0
// #define SUPPORT_TIMER_FD 0
// #endif



#define SUPPORT_PYTHON 0
#define SUPPORT_REDIS 0
#define SUPPORT_MYSQL_DB 0
#define SUPPORT_MQ 0
#define SUPPORT_OPENSSL 0
#define SUPPORT_SQLITE  0
//系统头文件
#include <algorithm>
#include <array>
#include <atomic> //c++11
#include <cassert>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <float.h>
#include <functional>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <mutex>
#include <random>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string>
#include <thread>
#include <time.h>
#include <unordered_map>
#include <vector>
using namespace std;

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2def.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "wsock32")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Advapi32.lib")
#elif defined(Cygwin)
#include <arpa/inet.h>
#include <dirent.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <ftw.h>
#elif defined(MAC)
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <ftw.h>
#include <ifaddrs.h>
#else
#include <linux/hdreg.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <ftw.h>
#include <ifaddrs.h>
#include <linux/sockios.h>
#endif



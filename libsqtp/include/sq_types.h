/**
Copyright (c) 2003-2015 
@brief 定义sqtp平台使用的数据类型


@author 
*/
#pragma once
#include "sq_define.h"

//定义socket 描述符
#if defined(LINUX)||defined(MAC)
typedef int fd_t;
enum { invalid_fd = -1 };

#endif
#if defined(WINDOWS)
typedef SOCKET fd_t;
enum { invalid_fd = (fd_t)INVALID_SOCKET };
#endif

typedef uint64_t time_val_t;

const int tid_platform_connect = 2000;
const int tid_platform_disconnect = 2001;

enum {
	EVT_READ = 2,
	EVT_WRITE = 4,
	EVT_ACCEPT = 8,
};


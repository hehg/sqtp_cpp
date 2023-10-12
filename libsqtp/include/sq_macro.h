/**
Copyright (c) 2003-2015 
@brief 预定义


@author 
*/
#pragma once

#include "sq_define.h"
#include <stddef.h>
///获取成员的大小
#define MEMB_SIZEOF(st_type, member) sizeof(((st_type *)0)->member)
///获取成员的地址
#define MEMB_CHARPTR(ptr, st_type, member)  ((char *)ptr + offsetof(st_type, member))

inline void sq_zero(void*a, int s) { memset(a, 0, s); }
#define safe_delete(a) { if (a) { delete a; }a = nullptr; }
#define safe_delete_array(a){ if(a)delete[] a; a = nullptr;}
inline void safe_strcpy(char*des, size_t des_len, const char*src)
{
	size_t src_len = strlen(src) + 1;
    size_t real_len = (src_len > des_len ? des_len : src_len);
	
	strncpy(des, src, real_len);
	des[real_len - 1] = '\0';
}

#define DISABLE_COPY_MOVE_AND_ASSIGN(type_name)                                 \
  type_name(const type_name &) = delete;                                         \
  void operator=(const type_name &) = delete

#if defined(LINUX)
#define SQTP_ALIGNMENT(n)      __attribute__ ((aligned (n)))
#define SQTP_BARRIER		   __asm__ __volatile__ ( "" ::: "memory" )


#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#define GET_LAST_SOCK_ERROR() errno
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#endif

#if defined(WINDOWS)
#define SQTP_ALIGNMENT(n)     __declspec( align(n) )
#define SQTP_BARRIER          std::atomic_thread_fence( std::memory_order_acq_rel )

#define likely(x) (x)
#define unlikely(x) (x)

#define GET_LAST_SOCK_ERROR() WSAGetLastError()
#endif

#if defined(__APPLE__)
#define GET_LAST_SOCK_ERROR() errno
#endif


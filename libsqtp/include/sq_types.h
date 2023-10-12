/**
Copyright (c) 2003-2015 
@brief 定义sqtp平台使用的数据类型


@author 
*/
#pragma once
#include "sq_define.h"

//定义socket 描述符
#if defined(LINUX)||defined(__APPLE__)
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


 enum class field_type_t : uint8_t
    {
        type_number_begin,
        type_uint8 = 1, //integer
        type_int8,
        type_char,
        type_bool,
        type_uint16,
        type_int16,
        type_uint32,
        type_int32,
        type_uint64,
        type_int64,
        type_number_end,

        type_real_begin,
        type_float32, //real
        type_float64,
        type_real_end,
        
        type_text_begin,
        type_char_array,
        type_text_end,

        type_array_begin,
        type_int32_array,
        type_float64_array, 
        type_array_end,
       
    };
    enum class field_class_t : uint8_t
    {
        class_none = 0,
        class_integer,
        class_boolean,
        class_text,
        class_real,
        class_array,
    };


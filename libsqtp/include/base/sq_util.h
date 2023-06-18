/**
Copyright (c) 2003-2015 
@brief 工具类


@author 
*/
#pragma once
#include "sq_define.h"
#include <math.h>
namespace sq
{
	class sq_util
	{
	public:

		///是否按字节对齐,align 必须是2的整数倍
		template <typename T>
		static inline bool is_aligned(T const * p, size_t align)
		{
			return (((uintptr_t)p) & uintptr_t(align - 1)) == 0;
		}
		

		/** 内存对齐方式申请内存
		@param size 申请内存大小
		@param align 对齐值，必须是2的幂
		@return 申请内存的地址
		*/
		static void * aligned_malloc(size_t size,size_t align=64);
		/**释放aligned_malloc申请的内存
		*/
		static void aligned_free(void * mem);

		inline static void sleep_ms(unsigned int ms)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		}
		inline static void sleep_us(unsigned int us)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(us));
		}

		static string get_env_val(string name);

		static std::string check_file_format(const char*path);

		static int conver_charset(const char*from_charset,const char*to_charset,
		char*inbuf,size_t inlen,char*outbuf,size_t outlen);
	};
}

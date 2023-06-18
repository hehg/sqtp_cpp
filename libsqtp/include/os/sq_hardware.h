/**
Copyright (c) 2003-2015
@brief 硬件相关的各个功能封装


@author
*/

#include "sq_define.h"
namespace sq
{
	/** 获取指定网卡的mac地址
	@param eth  --linux 平台传递网卡名称
				--win 平台传递ip地址
	*/
	static void get_mac_address(const char *eth, std::string &address);

	///获取cpu内核个数
	static inline int get_cpu_core_count()
	{
		return std::thread::hardware_concurrency();
	}
	///获取硬盘序列号
	static void get_disk_serial_num(string &serial);
}

/**
Copyright (c) 2003-2015 
@brief 日期时间操作类


@author 
*/
#pragma once
#include "sq_define.h"
#ifdef WINDOWS
struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};
int gettimeofday(timeval *tv, timezone *tz);
#endif
inline void os_sleep_ms(int ms){
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
namespace sq{
    struct sq_time
    {
        //当前时间
        int m_year;
        short m_month;
        short m_day;
        short m_hour;
        short m_minute;
        short m_second;
        int   m_msec;
        //总时间
        int64_t m_us;
    };
	/**
	* @brief  date_time 对象代表了当前系统时间，提供与时间相隔的各种功能
	*/
	class date_time
	{
	public:
	    date_time(int year,int month,int day,int hour=0,int minute=0,int second=0,
		int microsecond=0);
		/**
		 * @brief 获取当前时间
		*/
		static date_time now();
		/**
		 * @brief 从纳秒时间戳构造
		 */
		static date_time make_from_timestamp(uint64_t time_stamp);

		/**
		 * @brief 将当前时间以字符串的格式输出
		 * @param format 格式设置，目前支持的格式有：
					YYYYMMDD
					YYYY-MM-DD
					YYYY-MM-DD HH:mm:SS.sss
					HH:mm:SS
					HH:mm:SS.sss
					默认格式是YYYY-MM-DD HH:mm:SS.sss
		@return 输出是格式化后的字符串
		*/
		string format(string format); 
		void   format(char *buf, int len, string format);
		/**判断周几
		 * 返回  1~7 代表 周一~周日
		 */
		int weekday();
		/** 当前日期增加一天
		 */
		date_time &add_day(int n);
		
		int diff_days(const date_time &o);
		/** 获取当前系统时间，单位ms
		@return 当前时间的毫秒表示
		*/
        static uint64_t get_now_ms();
        static uint64_t get_now_us();
		static uint64_t  get_now_ns();
		
        static void get_sq_time(sq_time& time);
		void update_now_time();
	private:
		//格式20210918 这种格式构建对象
		//explicit date_time(int date);
		//date_time(int y, int m, int d);
	public:
		int m_year;
		int m_month;
		int m_day;
		int m_hour;
		int m_minute;
		int m_second;
		int m_msec;

    private:
		int m_ord;
	};
}
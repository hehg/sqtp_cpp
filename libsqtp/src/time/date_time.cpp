#include "time/date_time.h"
#include "sq_macro.h"

#ifdef WINDOWS
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64

int gettimeofday(timeval *tv, timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres /= 10; /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif
namespace sq
{
	static int DI4Y = 365 * 4 + 1;
	static int DI100Y = 25 * DI4Y - 1;
	static int DI400Y = 4 * DI100Y + 1;
	static int Days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static int Days_before_month[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	//static int Y = 1970, M = 1, D = 1;
	
	bool isLeapYear(int year)
	{
		return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	}
	int daysBeforeYear(int year)
	{
		--year;
		return 365 * year + year / 4 + year / 400 - year / 100;
	}
	int daysBeforeMonth(int year, int month)
	{
		return Days_before_month[month] + (int)(isLeapYear(year) && month > 2);
	}
	int ymd2ord(int y, int m, int d)
	{
		return daysBeforeYear(y) + daysBeforeMonth(y, m) + d;
	}
	static int s_offset = ymd2ord(1970, 1, 1) - 1;
	void ord2ymd(int m_ord, int &y, int &m, int &d)
	{
		int n = m_ord + s_offset - 1;
		//	cout <<"n = " <<n <<endl;
		int n400 = n / DI400Y;
		n = n % DI400Y;
		int n100 = n / DI100Y;
		n = n % DI100Y;
		int n4 = n / DI4Y;
		n = n % DI4Y;
		int n1 = n / 365;
		n = n % 365;
		y = 400 * n400 + 100 * n100 + 4 * n4 + n1 + 1;
		if ((n1 == 4 || n100 == 4) && n == 0)
		{
			--y;
			m = 12;
			d = 31;
			return;
		}
		m = (n + 50) >> 5;
		int preceding = daysBeforeMonth(y, m);
		if (preceding > n)
		{
			--m;
			preceding -= Days_in_month[m] + (int)(isLeapYear(y) && m == 2);
		}
		//	cout <<"preceding = " <<preceding <<" n = " <<n <<endl;
		//	cout <<"year: " <<y <<"Leapyear: " <<isLeapYear(y) <<"s_offset :" <<sq_date::s_offset <<endl;
		d = n - preceding + 1;
	}

	
	//===============date_time==================

	 date_time::date_time(int year,int month,int day,int hour,int minute,int second,
		int microsecond)
		{
			m_year=year;
			m_month=month;
			m_day=day;
			m_hour=hour;
			m_minute=minute;
			m_second=second;
			m_msec=microsecond;
			m_ord = ymd2ord(m_year, m_month, m_day) - s_offset;
		}
	// date_time::date_time(int date)
	// {
	// 	m_year = date / 10000;
	// 	m_month = date % 10000 / 100;
	// 	m_day = date % 100;
	// 	m_ord = ymd2ord(m_year, m_month, m_day) - s_offset;
	// }
	// date_time::date_time(int y, int m, int d)
	// {
	// 	m_ord = ymd2ord(y, m, d) - s_offset;
	// 	m_year = y;
	// 	m_month = m;
	// 	m_day = d;
	// }

	
	date_time date_time::now()
	{
		date_time dt(0,0,0);
		dt.update_now_time();
		
		return dt;
	}
	date_time date_time::make_from_timestamp(uint64_t time_stamp)
	{
		uint64_t t = time_stamp / 1000000000;
		tm my_tm;

#ifdef WINDOWS
		localtime_s(&my_tm, (const time_t *)&t);
#else
		localtime_r((const time_t *)&t, &my_tm);
#endif
		date_time dt(my_tm.tm_year + 1900,
		my_tm.tm_mon + 1,
		my_tm.tm_mday,
		my_tm.tm_hour,
		my_tm.tm_min,
		my_tm.tm_sec,
		(time_stamp / 1000000) % 1000);
		
		return dt;
		
	}
	void date_time::update_now_time()
	{
#ifdef WINDOWS
		SYSTEMTIME system;

		GetLocalTime(&system);

		m_year = system.wYear;
		m_month = system.wMonth;
		m_day = system.wDay;
		m_hour = system.wHour;
		m_minute = system.wMinute;
		m_second = system.wSecond;
		m_msec = system.wMilliseconds; //毫秒

#else
		struct timespec CurrentTime;
		struct tm _tx;
		clock_gettime(CLOCK_REALTIME, &CurrentTime);
		localtime_r(&CurrentTime.tv_sec, &_tx);

		m_year = _tx.tm_year + 1900;
		m_month = _tx.tm_mon + 1;
		m_day = _tx.tm_mday;
		m_hour = _tx.tm_hour;
		m_minute = _tx.tm_min;
		m_second = _tx.tm_sec;
		m_msec = CurrentTime.tv_nsec / 1000000;
#endif
		m_ord = ymd2ord(m_year, m_month, m_day) - s_offset;
	}
	string date_time::format(string format)
	{
		string str;
		char buf[100] = {0};
		if (format == "YYYYMMDD")
		{
			snprintf(buf, sizeof(buf), "%d%02d%02d", m_year, m_month, m_day);
		}
		else if (format == "YYYY-MM-DD")
		{
			snprintf(buf, sizeof(buf), "%d-%02d-%02d", m_year, m_month, m_day);
		}
		else if (format == "YYYY-MM-DD HH:mm:SS.sss")
		{
			snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d", m_year, m_month, m_day, m_hour, m_minute, m_second, m_msec);
		}
		else if (format == "HH:mm:SS")
		{
			snprintf(buf, sizeof(buf), "%02d:%02d:%02d", m_hour, m_minute, m_second);
		}
		else if (format == "HH:mm:SS.sss")
		{
			snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", m_hour, m_minute, m_second, m_msec);
		}
		else if (format == "YYYYMMDD_HH_mm_SS")
		{
			snprintf(buf, sizeof(buf), "%d%02d%02d_%02d_%02d_%02d", m_year, m_month, m_day, m_hour, m_minute, m_second);
		}
		else
		{
			snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d", m_year, m_month, m_day, m_hour, m_minute, m_second, m_msec);
		}
		str = buf;
		return str;
	}

	void date_time::format(char *buf, int len, string form)
	{
		string str = format(form);
		if (buf)
		{
			safe_strcpy(buf, len, str.c_str());
		}
	}

	

	uint64_t date_time::get_now_ms()
	{
		//auto now_time = chrono::steady_clock::now();
		auto now_time = chrono::system_clock::now();
		uint64_t now = (chrono::duration_cast<chrono::milliseconds>(now_time.time_since_epoch())).count();
		return now;
	}
	uint64_t date_time::get_now_us()
	{
		//auto now_time = chrono::steady_clock::now();
		auto now_time = chrono::system_clock::now();
		uint64_t now = (chrono::duration_cast<chrono::microseconds>(now_time.time_since_epoch())).count();
		return now;
	}
	uint64_t date_time::get_now_ns()
	{
		auto now_time = chrono::system_clock::now();
		uint64_t now = (chrono::duration_cast<chrono::nanoseconds>(now_time.time_since_epoch())).count();
		return now;
		//return chrono::high_resolution_clock::now().time_since_epoch().count();
	}

	void date_time::get_sq_time(sq_time &time)
	{
#ifdef WINDOWS
		SYSTEMTIME system;

		GetLocalTime(&system);

		time.m_year = system.wYear;
		time.m_month = system.wMonth;
		time.m_day = system.wDay;
		time.m_hour = system.wHour;
		time.m_minute = system.wMinute;
		time.m_second = system.wSecond;
		time.m_msec = system.wMilliseconds; //毫秒

#else
		struct timeval CurrentTime;
		struct tm _tx;
		gettimeofday(&CurrentTime, NULL);
		localtime_r(&CurrentTime.tv_sec, &_tx);
		CurrentTime.tv_usec /= 1000; //换为毫秒
		time.m_year = _tx.tm_year + 1900;
		time.m_month = _tx.tm_mon + 1;
		time.m_day = _tx.tm_mday;
		time.m_hour = _tx.tm_hour;
		time.m_minute = _tx.tm_min;
		time.m_second = _tx.tm_sec;
		time.m_msec = CurrentTime.tv_usec;
#endif
	}

	
	date_time &date_time::add_day(int n)
	{
		m_ord += n;
		ord2ymd(m_ord, m_year, m_month, m_day);
		return *this;
	}
	int date_time::weekday()
	{
		/*//方法一：蔡勒公式,公式中的除法取整数。

        符号意义：  w:星期 eg:{0=星期日，1=星期一，...}
                    c:年数前两位 eg:{2015 c=20}
                    y:年数后两位 eg:{2015 y=15}
                    m:月 eg:{2015年2月28日 m=14,此时y=2014}  注意 1,2月要当成上一年的13,14月计算
                    d:日 eg:{28日 d=28}
        ① w=(y + y/4 + c/4 - 2*c + (26 * (m+1))/10 + d - 1) %7
      或② w=(c/4 - 2*c + y + y/4 + (13 * (m+1))/5 + d -1) %7

    unsigned  w, c, y, m, d;
    c = Year / 100;
    y = Year % 100;
    m = Month;
    if (Month == 1 || Month == 2)
    {
        y--;
        m += 12;
    }
    d = Day;
    w = (c / 4 - 2 * c + y + y / 4 + (13 * (m + 1)) / 5 + d - 1) % 7;
    return _WeekDay_CHS[w];
    */

		/*//方法二：基姆拉尔森公式,公式中的除法取整数

        符号意义：  w:星期 eg:{6=星期日，0=星期一，...}
                    y:年数 eg:{2015 y=2015}
                    m:月 eg:{2015年2月28日 m=14,此时y=2014}  注意 1,2月要当成上一年的13,14月计算
                    d:日 eg:{28日 d=28}
         w=(d + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400) %7

    unsigned  w, y, m, d;
    y = Year;
    m = Month;
    if (Month == 1 || Month == 2)
    {
        y--;
        m += 12;
    }
    d = Day;
    w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7 +1;
    //std::cout << w << std::endl;
    return _WeekDay_CHS[w];
    */

		/*//方法三：基数法,公式中的除法取整数

        1.年基数yb：
            平年=1，
            闰年=2。
        2.月基数mb：该月之前(不含该月)的每月天数的总和除以7取余
            平年={0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}，
            闰年={0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6}。
        3.符号意义：
            w:星期 eg:{0=星期日，1=星期一，...}
            y:年份 eg:{2015年3月4日 y=2015}
            yb:年基数 eg:{2015年3月4日 yb=1}
            mb:月基数 eg:{2015年3月4日 mb=3}
            d:日 eg:{2015年3月4日 d=4}
        w=(y + y/4 + y/400 - y/100 - yb + mb + d)%7
    */
		int NLY[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
		int LY[12] = {0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
		unsigned y, yb, mb, d, w;
		if (isLeapYear(m_year))
		{
			yb = 2;
			mb = LY[m_month - 1];
		}
		else
		{
			yb = 1;
			mb = NLY[m_month - 1];
		}
		y = m_year;
		d = m_day;
		w = (y + y / 4 + y / 400 - y / 100 - yb + mb + d) % 7;
		if (w == 0)
			return 7;
		return w;
	}
	int date_time::diff_days(const date_time &o)
	{
		return m_ord - o.m_ord;
	}

}

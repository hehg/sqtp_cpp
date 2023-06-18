#include "sq_convert.h"
namespace sq {
	const static string s_order_status_name[]{
		"unknow",
        "insert_submit",
        "cancel_submit",
		"reject",
        "cancel_reject",
        "accept",
		"part_trade",
        "all_trade",
        "canceled",
        "cancel_reject_by_broker"
	};
    uint64_t datetime_to_int(const char *time)
    {
        int hh, mm, ss, sss;
        hh = mm = ss = sss = 0;
        sscanf(time, "%d:%d:%d.%d", &hh, &mm, &ss, &sss);
        if (hh <= 17)
            return 1000 * ((hh + 24) * 3600 + mm * 60 + ss) + sss;
        else
            return 1000 * (hh * 3600 + mm * 60 + ss) + sss;
    }
    std::string int_to_datetime(uint64_t time)
    {
        //
        int h, m, s, ss;
        ss = time % 1000;
        s = (time / 1000);
        h = s / 3600;
        m = (s - h * 3600) / 60;
        s = s - h * 3600 - m * 60;
        if (h >= 24)
            h = h - 24;
        char buf[32] = {0};
        sprintf(buf, "%02d:%02d:%02d.%03d", h, m, s, ss);
        return string(buf);
    }
    const char *get_order_status_name(order_status_t s)
    {
        if (s>=0 && s<=9) {
			return s_order_status_name[s].c_str();
		}
		else {
			return s_order_status_name[0].c_str();
		}
    }
    void split_quot_time(const char*time,int &hh,int&mm,int&ss,int&sss)
    {
        const static int len1 = 12;//("12:12:00.000");
        const static int len2 = 8;//("12:12:00");

        if (strlen((const char*)time) == len1) {
            hh = mm = ss = sss = 0;
            sscanf(time, "%d:%d:%d.%d", &hh, &mm, &ss, &sss);
           
        }
        else if (strlen((const char*)time) == len2)
        {
            hh = mm = ss = sss = 0;
            sscanf(time, "%d:%d:%d", &hh, &mm, &ss);
        }
        else {
            hh = mm = ss = sss = 0;
            sscanf(time, "%d:%d:%d", &hh, &mm, &ss);
        }
    }
}
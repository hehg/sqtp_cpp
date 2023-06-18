#ifndef SQ_CONVERT_H // header guards
#define SQ_CONVERT_H

#include "sq_define.h"
#include <math.h>
#include "sq_type_define.h"
namespace sq
{


	/**将时间戳转化为整数表示的时间(毫秒)
	*/
    uint64_t datetime_to_int(const char* time);
    std::string int_to_datetime(uint64_t time);
    
    /**把价格变成tick的整数倍
    */
    inline price_t  unit_price(price_t price,price_t tick) {
        return floor(price / tick)*tick;
    }
	const char* get_order_status_name(order_status_t s);

    void split_quot_time(const char*time,int &h,int&m,int&s,int&ms);
}

#endif
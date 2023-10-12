/**
 * @brief 业务库 工具类
*/
#pragma once
#include "sq_type_define.h"
namespace  sq
{
    inline bool is_reject_statues(order_status_t s)
    {
        return (s == order_status_t::status_insert_reject || s == order_status_t::status_cancel_reject ||
                s == order_status_t::status_cancel_reject_by_broker);
    }
}
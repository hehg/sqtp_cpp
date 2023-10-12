#include "sq_struct_des.h"
#include "sq_struct_def.h"

//这些代码是工具自动生成出来的

/**
 * @brief 定义描述类的各个结构体成员
*/
namespace sq
{

    SQ_DES_DEFINE_BEGIN(sq_quot)
    {
        ADD_FD_UINT16(sq_quot, tid);
        ADD_FD_UINT32(sq_quot, pkg_no);
        ADD_FD_UINT8(sq_quot, type);
        ADD_FD_STR(sq_quot, market);
        ADD_FD_STR(sq_quot, variety);
        ADD_FD_UINT32(sq_quot, contract_idx);
        ADD_FD_STR(sq_quot, contract);
        ADD_FD_FLOAT64(sq_quot, price);
        ADD_FD_INT32(sq_quot, match_qty);
        ADD_FD_FLOAT64(sq_quot, turnover);
        ADD_FD_FLOAT64(sq_quot, interest);
        ADD_FD_FLOAT64_ARRAY(sq_quot, mbl_buy_price, 5);
        ADD_FD_INT_ARRAY(sq_quot, mbl_buy_qty, 5);
        ADD_FD_FLOAT64_ARRAY(sq_quot, mbl_sell_price, 5);
        ADD_FD_INT_ARRAY(sq_quot, mbl_sell_qty, 5);
        ADD_FD_FLOAT64(sq_quot, open);
        ADD_FD_FLOAT64(sq_quot, close);
        ADD_FD_FLOAT64(sq_quot, high);
        ADD_FD_FLOAT64(sq_quot, low);
        ADD_FD_FLOAT64(sq_quot, limit_up);
        ADD_FD_FLOAT64(sq_quot, limit_down);
        ADD_FD_FLOAT64(sq_quot, pre_close);
        ADD_FD_INT32(sq_quot, multi);
        ADD_FD_UINT64(sq_quot, mstime);
        ADD_FD_UINT64(sq_quot, recv_time);
        ADD_FD_STR(sq_quot, date);
        ADD_FD_STR(sq_quot, time);
    }

    SQ_DES_DEFINE_BEGIN(sq_req_order)
    {
        ADD_FD_STR(sq_req_order, user_id);
        ADD_FD_INT64(sq_req_order, sq_local_id);
        ADD_FD_STR(sq_req_order, market);
        ADD_FD_STR(sq_req_order, contract);
        ADD_FD_UINT8(sq_req_order, direction);
        ADD_FD_UINT8(sq_req_order, offset);
        ADD_FD_FLOAT64(sq_req_order, price);
        ADD_FD_FLOAT64(sq_req_order, stop_price);
        ADD_FD_INT32(sq_req_order, qty);
        ADD_FD_UINT32(sq_req_order, type);
        ADD_FD_UINT8(sq_req_order, product_type);
    }

    SQ_DES_DEFINE_BEGIN(sq_order_record)
    {
        ADD_FD_STR(sq_order_record, user_id);
        ADD_FD_INT64(sq_order_record, sq_local_id);
        ADD_FD_STR(sq_order_record, offer_id);
        ADD_FD_STR(sq_order_record, contract);
        ADD_FD_STR(sq_order_record, market);
        ADD_FD_UINT8(sq_order_record, direction);
        ADD_FD_UINT8(sq_order_record, offset);
        ADD_FD_INT32(sq_order_record, qty);
        ADD_FD_INT32(sq_order_record, match_qty);
        ADD_FD_FLOAT64(sq_order_record, match_price);
        ADD_FD_INT32(sq_order_record, match_total);
        ADD_FD_INT32(sq_order_record, cancel_qty);
        ADD_FD_UINT8(sq_order_record, status);
        ADD_FD_UINT8(sq_order_record, pre_status);
        ADD_FD_FLOAT64(sq_order_record, price);
        ADD_FD_INT32(sq_order_record, err_code);
        ADD_FD_STR(sq_order_record, err_msg);
        ADD_FD_INT64(sq_order_record, insert_time);
        ADD_FD_UINT32(sq_order_record, type);
        ADD_FD_UINT8(sq_order_record, product_type);
        ADD_FD_BOOL(sq_order_record, is_finish);
        ADD_FD_STR(sq_order_record, sys_id);
        ADD_FD_INT64(sq_order_record, cancel_time);
    }

    SQ_DES_DEFINE_BEGIN(sq_order_match_ntf)
    {
        ADD_FD_STR(sq_order_match_ntf, user_id);
        ADD_FD_UINT64(sq_order_match_ntf, match_id);
        ADD_FD_INT64(sq_order_match_ntf, sq_local_id);
        ADD_FD_STR(sq_order_match_ntf, offer_id);
		ADD_FD_STR(sq_order_match_ntf, contract);
        ADD_FD_FLOAT64(sq_order_match_ntf, match_price);
        ADD_FD_INT32(sq_order_match_ntf, match_qty);
        ADD_FD_UINT8(sq_order_match_ntf, direction);
        ADD_FD_UINT8(sq_order_match_ntf, offset);
        ADD_FD_STR(sq_order_match_ntf, date);
        ADD_FD_STR(sq_order_match_ntf, time);
    }

    SQ_DES_DEFINE_BEGIN(sq_order_state_ntf)
    {
        ADD_FD_STR(sq_order_state_ntf, user_id);
        ADD_FD_INT64(sq_order_state_ntf, sq_local_id);
		ADD_FD_STR(sq_order_state_ntf, contract);
        ADD_FD_UINT8(sq_order_state_ntf, status);
        ADD_FD_INT32(sq_order_state_ntf, sys_id);
        ADD_FD_INT32(sq_order_state_ntf, cancel_qty);
        ADD_FD_UINT8(sq_order_state_ntf, direction);
        ADD_FD_UINT8(sq_order_state_ntf, offset);
    }
}
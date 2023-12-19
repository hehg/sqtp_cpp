#include "sq_struct_des.h"
#include "sq_struct_def.h"

// 这些代码是工具自动生成出来的

/**
 * @brief 定义描述类的各个结构体成员
 */
namespace sq
{
    #define add_fld(f,t)   add_fld_des(#f, t,sizeof(base_class::f), offsetof(base_class, f))

    #define add_field_str(f)   add_fld(f,field_type_t::type_char_array)
    #define add_field_int8(f)   add_fld(f,field_type_t::type_int8)
    #define add_field_char(f)   add_fld(f,field_type_t::type_char)
    #define add_field_uint8(f)   add_fld(f,field_type_t::type_uint8)
    #define add_field_int16(f)   add_fld(f,field_type_t::type_int16)
    #define add_field_uint16(f)   add_fld(f,field_type_t::type_uint16)
    #define add_field_int32(f)   add_fld(f,field_type_t::type_int32)
    #define add_field_uint32(f)   add_fld(f,field_type_t::type_uint32)
    #define add_field_int64(f)   add_fld(f,field_type_t::type_int64)
    #define add_field_uint64(f)   add_fld(f,field_type_t::type_uint64)

    #define add_field_float32(f)   add_fld(f,field_type_t::type_float32)
    #define add_field_float64(f)   add_fld(f,field_type_t::type_float64)
    #define add_field_int_array(f)   add_fld(f,field_type_t::type_int32_array)
    #define add_field_float64_array(f)   add_fld(f,field_type_t::type_float64_array)

   #define add_field_bool(s, f)       add_fld(f, field_type_t::type_bool)
    sq_contract_desc::sq_contract_desc()
    {
        using base_class = sq_contract;
        add_field_str(contract);
        add_field_str(variety);
        add_field_str(market);
        add_field_float64(tick);
        add_field_uint8(product_type);
        add_field_float64(unit);
        add_field_int32(multiple);
        add_field_int32(max_limit_order_vol);
        add_field_int32(min_limit_order_vol);
        add_field_int32(max_market_order_vol);
        add_field_int32(min_market_order_vol);
        add_field_int32(max_pos_vol);
        add_field_str(trade_date_begin);
        add_field_str(trade_date_end);
        add_field_str(deliv_month);
        add_field_str(deliv_year);
        add_field_str(deliv_date_begin);
        add_field_str(deliv_date_end);
        add_field_str(expiry_date);
        add_field_int32(status);
        add_field_float64(benchmark_price);
        add_field_str(base_contract);
        add_field_char(call_put);
        add_field_float64(strike_price);

    }

    sq_quot_desc::sq_quot_desc()
    {
        using base_class = sq_quot;
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

    sq_req_order_desc::sq_req_order_desc()
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

    sq_order_record_desc::sq_order_record_desc()
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

    sq_order_match_ntf_desc::sq_order_match_ntf_desc()
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

    sq_order_state_ntf_desc::sq_order_state_ntf_desc()
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

    sq_qry_contract_req_desc::sq_qry_contract_req_desc()
    {
        using base_class = sq_qry_contract_req;
        add_field_str(market_id);
        add_field_str(contract_id);
    }
}
#pragma once
#include "sq_struct_def.h"
#include "sq_biz_table_base.h"
using namespace std;
namespace sq {
    /** 订单管理

    根据订单的状态，为应用层产生不同的报文
    目前底层只产生订单状态通知和成交回报通知

    自己维护了各个订单的状态，当收到订单状态通知时，更新对应订的状态
    然后根据这些信息判断订单是否确认，为应用层产生订单回报通知
    和撤单回报通知
    */

    class sq_order_table : public sq_biz_table_base<int64_t, sq_order_record>
    {
    public:
      sq_order_table(){};
      ~sq_order_table(){};

      sq_order_record* on_order_state(order_id_t local_id, sys_order_id_t &sys_id, order_status_t status, qty_t cancel_qty);
      sq_order_record* on_order_match(order_id_t local_id, price_t match_price, qty_t match_qty);
    };
}
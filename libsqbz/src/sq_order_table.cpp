#include "sq_order_table.h"
#include "time/date_time.h"
namespace sq {

    sq_order_record* sq_order_table::on_order_state(order_id_t local_id,sys_order_id_t& sys_id,order_status_t status,
    qty_t cancel_qty)
    {
        sq_order_record*order=find(local_id);

        if(order->sys_id.empty()){
            order->sys_id=sys_id;
        }
        if(order->status==order_status_t::status_cancel){
            order->cancel_qty=cancel_qty;
        }
        order->change_status(status);

        return order;
    }
	sq_order_record* sq_order_table::on_order_match(order_id_t local_id, price_t match_price, qty_t match_qty)
	{
		sq_order_record*order = find(local_id);
		order->match_price = match_price;
		order->match_qty = match_qty;
		order->match_total += match_qty;

		order->change_status(order->match_total == order->qty ?
			status_all_trade : status_part_trade);
        return order;
	}
}
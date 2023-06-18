#pragma once
#include <string>
#include "sq_biz_table_base.h"
#include "sq_type_define.h"
#include "sq_contract_posi_table.h"
namespace sq
{
    class sq_posi_table :public sq_biz_table_base<string, sq_contract_posi_table> {
        public:

        int on_place_order(const char*contract,price_t price, qty_t vol, bs_flag_t bs, offset_t offset);
        void on_order_match(const char*contract,price_t price,qty_t qty,bs_flag_t direction,offset_t offset);
        void on_order_state(const char*contract,order_status_t status,qty_t qty,qty_t cancel_qty,bs_flag_t direction, offset_t offset);

		double get_profit();
    };
}
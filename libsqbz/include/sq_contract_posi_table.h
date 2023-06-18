#pragma once
#include <vector>
#include "sq_type_define.h"
#include "sq_struct_def.h"
namespace sq
{
   
    // 合约持仓表
    class sq_contract_posi_table
    {

    public:
        sq_contract_posi_table(const char *contract);
        ~sq_contract_posi_table();

        void dump();
        //
        int on_place_order(price_t price, qty_t vol, bs_flag_t direction, offset_t offset);
        void on_order_match(price_t price,qty_t vol,bs_flag_t direction,offset_t offset);
        void on_order_state(order_status_t status,qty_t qty,qty_t cancel_qty,bs_flag_t direction, offset_t offset);

		double get_float_profit(double price);
		sq_contract_posi m_sq_contract_posi;

        // 持仓明细
        std::vector<position_detail> m_buy_posi_detail_list;
        std::vector<position_detail> m_sell_posi_detail_list;
    };
}
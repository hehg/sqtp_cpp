#include "sq_posi_table.h"

namespace sq
{

         int sq_posi_table::on_place_order(const char*contract,price_t price, qty_t vol, bs_flag_t bs, offset_t offset)
         {
            sq_contract_posi_table*tb= find(contract);
			if (tb == nullptr){
				std::string key = contract;
                add(key,new sq_contract_posi_table(contract));
                tb= find(contract);
            }
            return tb->on_place_order(price,vol,bs,offset);
            
         }
        void sq_posi_table::on_order_match(const char*contract,price_t match_price,qty_t match_qty,
        bs_flag_t direction,offset_t offset)
        {
            sq_contract_posi_table*tb= find(contract);
            if(tb==nullptr){
                printf("contract=%s\n",contract);
                assert(tb);
            }
          
            tb->on_order_match(match_price,match_qty,direction,offset);
        }
        void sq_posi_table::on_order_state(const char*contract,order_status_t status,qty_t qty,qty_t cancel_qty,
        bs_flag_t direction, offset_t offset)
        {
            sq_contract_posi_table*tb= find(contract);
            if(tb){
                tb->on_order_state(status,qty,cancel_qty,direction,offset);
            }
        }

		double sq_posi_table::get_profit()
		{
			double profit = 0;
			auto it = m_Table.begin();
			for (; it != m_Table.end(); ++it)
			{
				profit+=(*it)->m_sq_contract_posi.m_close_profit;
			}
			return profit;
		}
}
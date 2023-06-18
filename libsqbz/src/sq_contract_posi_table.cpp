#include "sq_contract_posi_table.h"
#include "log/sq_logger.h"
namespace sq
{

    sq_contract_posi_table::sq_contract_posi_table(const char *contract)
    {
		m_sq_contract_posi.m_contract = contract;
    }

    sq_contract_posi_table::~sq_contract_posi_table()
    {
    }
    void sq_contract_posi_table::dump()
    {
        log_info("sq_contract_posi_table,buy_opening={},sell_opening={},buy_closing={},sell_closing={},buy_posi={},sell_posi={}\n", 
			m_sq_contract_posi.m_buy_opening,
			m_sq_contract_posi.m_sell_opening,
			m_sq_contract_posi.m_buy_closing,
			m_sq_contract_posi.m_sell_closing,
			m_sq_contract_posi.m_buy_posi,
			m_sq_contract_posi.m_sell_posi);
    }
    
    int sq_contract_posi_table::on_place_order(price_t price, qty_t qty, bs_flag_t bs, offset_t offset)
    {
        if (bs == f_buy && offset == f_open)
        {
			m_sq_contract_posi.m_buy_opening += qty;
        }
        else if (bs == f_sell && offset == f_open)
        {
			m_sq_contract_posi.m_sell_opening += qty;
        }
        else if (bs == f_buy && offset == f_close)
        {
			m_sq_contract_posi.m_buy_closing += qty;
        }
        else if (bs == f_sell && offset == f_close)
        {
			m_sq_contract_posi.m_sell_closing += qty;
        }
        return 0;
    }
     void sq_contract_posi_table::on_order_match(price_t price,qty_t qty,bs_flag_t direction,offset_t offset)
     {
        if (direction == f_buy && offset == f_open)
        {
			m_sq_contract_posi.m_buy_opening -= qty;
			m_sq_contract_posi.m_buy_posi += qty;

			m_sq_contract_posi.m_buy_turnover += price * qty;
            // 逐笔明细
            position_detail detail;
            detail.direction = direction;
            detail.open_price = price;
            detail.open_qty = qty;
            m_buy_posi_detail_list.push_back(detail);
        }
        else if (direction == f_sell && offset == f_open)
        {
			m_sq_contract_posi.m_sell_opening -= qty;
			m_sq_contract_posi.m_sell_posi += qty;

			m_sq_contract_posi.m_sell_turnover += price * qty;
            // 逐笔明细
            position_detail detail;
            detail.direction = direction;
            detail.open_price = price;
            detail.open_qty = qty;
            m_sell_posi_detail_list.push_back(detail);
        }
        else if (direction == f_buy && offset == f_close)
        {
			m_sq_contract_posi.m_buy_closing -= qty;
			m_sq_contract_posi.m_sell_posi -= qty;
            if (m_sq_contract_posi.m_sell_posi == 0)
            {
				m_sq_contract_posi.m_sell_turnover = 0;
            }
            else
            {
				m_sq_contract_posi.m_sell_turnover -= price * qty;
            }

            // 统计逐笔盈亏
            {
                int will_close_qty = qty;
                auto it = m_sell_posi_detail_list.begin();
                for (; it != m_sell_posi_detail_list.end(); ++it)
                {
                    int remain = it->open_qty - it->close_qty;
                    if (remain > 0)
                    {
                        // 可平量够用
                        if (remain >= will_close_qty)
                        {
                            it->close_qty += will_close_qty;
                            double profit = -(price - it->open_price) * will_close_qty;
                            it->profit += profit;

							m_sq_contract_posi.m_close_profit += profit;
                            will_close_qty = 0;
                        }
                        // 可平量不够了
                        else
                        {
                            it->close_qty += remain;
                            double profit = -(price - it->open_price) * remain;
                            it->profit += profit;
                            will_close_qty -= remain;

							m_sq_contract_posi.m_close_profit += profit;

                        }
                    }

                    if (will_close_qty == 0)
                    {
                        break;
                    }
                }
            }
        }
        else if (direction == f_sell && offset == f_close)
        {
			m_sq_contract_posi.m_sell_closing -= qty;
			m_sq_contract_posi.m_buy_posi -= qty;
        
            if (m_sq_contract_posi.m_buy_posi == 0)
            {
				m_sq_contract_posi.m_buy_turnover = 0;
            }
            else
            {
				m_sq_contract_posi.m_buy_turnover -= price * qty;
            }

            // 统计逐笔盈亏
            {
                int will_close_qty = qty;
                auto it = m_buy_posi_detail_list.begin();
                for (; it != m_buy_posi_detail_list.end(); ++it)
                {
                    int remain = it->open_qty - it->close_qty;
                    if (remain > 0)
                    {
                        // 可平量够用
                        if (remain >= will_close_qty)
                        {
                            it->close_qty += will_close_qty;
                            double profit = (price - it->open_price) * will_close_qty;
                            it->profit += profit;

							m_sq_contract_posi.m_close_profit += profit;

                           
                            will_close_qty = 0;
                        }
                        // 可平量不够了
                        else
                        {
                            it->close_qty += remain;
                            double profit = (price - it->open_price) * remain;
                            it->profit += profit;
                            will_close_qty -= remain;

							m_sq_contract_posi.m_close_profit += profit;

                        }
                    }

                    if (will_close_qty == 0)
                    {
                        break;
                    }
                }
            }
        }
     }
    
    void sq_contract_posi_table::on_order_state(order_status_t status,qty_t qty,qty_t cancel_qty,
    bs_flag_t direction, offset_t offset)
    {
        
        if (status == status_cancel || status == status_insert_reject)
        {
            int qty = 0;
            if (status == status_cancel)
            {
                qty = cancel_qty;
            }
            if (status == status_insert_reject)
            {
                qty = qty;
            }

            if (direction == f_buy && offset == f_open)
            {
				m_sq_contract_posi.m_buy_opening -= qty;
            }
            else if (direction == f_sell && offset == f_open)
            {
				m_sq_contract_posi.m_sell_opening -= qty;
            }
            else if (direction == f_buy && offset == f_close)
            {
				m_sq_contract_posi.m_buy_closing -= qty;
            }
            else if (direction == f_sell && offset == f_close)
            {
				m_sq_contract_posi.m_sell_closing -= qty;
            }
        }
    }


	double sq_contract_posi_table::get_float_profit(double price)
	{
		return 0;
	}
}
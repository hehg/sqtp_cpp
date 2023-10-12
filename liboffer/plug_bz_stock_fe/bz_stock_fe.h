#include "plug_base.h"
#include <thread>
#include "stock_big_order.h"
#include "stock_moment.h"
#include "stock_quote_price.h"
#include "sq_fe_struct.h"
namespace sq_plug
{

    class bz_stock_fe : public plug_base
    {
    public:
        bz_stock_fe();
        ~bz_stock_fe();
        int open();
        int close();
        void run();
        int put(uint16_t tid, char *data, uint16_t size);
        void handle_time_for_reocver();
        stock_fe_base* m_fes[3];
        int m_fes_size=3;
        stock_quote_price m_stock_quote_price;
        stock_moments m_stock_moments;
        stock_big_orders m_stock_big_order;
        std::unordered_map<std::string,int> m_support_stocks;
        bool	m_is_open_state = false; //交易状态
		int 	m_dest_tid;
        int     m_timer_interval=500; //ms
        uint64_t m_cur_timestamp=0;
        uint64_t m_last_time=0;
        int      m_int_time=0;
        stock_fe_result m_result;
    };
}
#pragma once
#include "sim_base.h"
#include <fstream>
#include <thread>
#include "sq_quot_file.h"
namespace sq_plug
{
    struct handup_info;
    
    /*记录下单那一时刻行情相关的信息*/
    struct  handup_order_info {
        sq_order_record*order; //订单信息
        qty_t   before_me;  //排在我前面的订单量
		int32_t  handup_pkg_no = 0;
		handup_info* handup = nullptr;
		
    };
	struct handup_info
	{
		handup_info() {
			
			m_total_qty = 0;
		}
		std::list<handup_order_info*> m_orders;
		qty_t  m_total_qty; //策略挂单总量
	};
    /**推导成交信息
    */
    class  driver_match_info
    {
    public:
        void driver_new_match(sq_quot*);
		void driver_new_handup_order(sq_quot* cur, sq_quot*next);

        double m_unit = 10;
        double m_tick = 5;

        double m_atp = 0;
        double m_mpb = 0;
        double m_mpa = 0;
        double m_mvb = 0;
        double m_mva = 0;

        double m_pre_turnover=0; //前成交额
        double m_pre_match_qty=0;//前成交量
        double m_pre_mid_price=0; //前一个中间价
		double m_cur_mid_price = 0; //当前中间价
        double m_pre_bid = 0;
        double m_pre_ask = 0;

		

		double m_AttB = 0;
		double m_AttS = 0;
    };
	
	struct match_result {
		order_id_t order_id;
		int        direction;
		price_t    match_price;
		qty_t      match_qty;
	};
    /** 基于行情推导的模拟撮合
    */
	class td_sim_driver :public sim_base
	{
	public:
		td_sim_driver();
		~td_sim_driver();
        int open();
        int close();
		int put(uint16_t tid, char* data,uint16_t size);
		
		void run();
        virtual void on_order_insert(sq_order_record*order)override;
		virtual void on_order_cancel(sq_order_record*order)override;
		virtual void match(sq_quot*quot, driver_match_info&driver);
		
	private:
		bool load_quot();
		bool read_quot();
		virtual void match_sim_cancel(sq_quot*quot, driver_match_info&driver, std::vector<match_result>&result);
		bool _exe_match_by_quot_sim_cancel(sq_order_record*order, sq_quot*quot, std::vector<match_result>&result);

        bool _exe_match_by_quot(sq_order_record*order, sq_quot*quot);
		
		int get_next_quot_info(price_t price, qty_t&next_val);
		int get_cur_quot_info(price_t price, qty_t&next_val);
		void  send_all_match(sq_order_record*order);
		void  handup_order(sq_order_record*order,qty_t new_order,qty_t beforeme);

		void  decrease_handup_qty(price_t price, int direction, qty_t vol);
    private:
        std::thread* m_eg=nullptr;
		bool   m_run_flag=false;

		string m_quot_file_path;
		bool   m_quot_file_have_header=true;
		string m_quot_file_header;
		
        string m_contract;
		std::vector<sq_quot*>   m_all_quot; //预读取的行情数据

		sq_quot*           m_pre_quot = nullptr;
		sq_quot*           m_cur_quot=nullptr;
		sq_quot*           m_next_quot=nullptr;

		int  m_cur_quot_idx = 0;
		int  m_pkg_no = 0;
		sq_quot_file_reader quot_reader;
		std::map<double, handup_info>m_buy_handup_info;
        std::map<double, handup_info>m_sell_handup_info;
       driver_match_info  m_driver; //计算当前行情与之前行情的成交
	   driver_match_info  m_driver_next;//计算当前行情与下一个行情的成交
	   double 	m_tick = 0;
	   double 	m_unit = 0;
	   double 	m_beforeme_fix = 10;
	   int 		m_beforeme_type = 0;
	   double 	m_beforeme_rate = 0.5;
       int     	m_match_price_type = match_price_otherside;
	   std::fstream  sim_log;
	   bool m_support_sim_log = false;
	   string m_sim_log_name = "sim_trade";
	   double m_new_order = 0; //竞争单
	   int  m_handup_case = 0;

	   double m_cancel_match_rate = 0.5;//撤单时可能成交的比例
	   std::vector<match_result> m_pre_match_result;//撤单预成交结果

	   int m_all_match_type=0;//全部成交时的处理策略 0--正常逻辑 1--全部对价
	   bool m_enable_best_match = false;//是否支持最优价触发成交
	};
    
}
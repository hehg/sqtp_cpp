#pragma once
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include "sq_type_define.h"
#include "sq_struct_def.h"
#include "sq_convert.h"
/**
 * @brief 撮合引擎模块，只负责订单撮合，产生撮合结果
*/
namespace sq
{
	class sq_match_core;
   

    /**
	 * 撮合内部的订单信息，只保留撮合需要的订单信息
	*/
	struct mc_order 
	{
		order_id_t 		order_id=0;		 		//交易所编号
		bs_flag_t 		direction;
		int 			price=0;			   	//委托价格
		qty_t 			qty=0;				   	//委托数量
		order_type_t 	type=0;			 		//订单类型
		bool            is_fin=false;
		//按价位排序
		bool operator <(const mc_order&left)const {
			//买排序
			if (direction == f_buy) {
				if (price > left.price)
					return true;
				else if (price < left.price)
					return false;
				else {
					//价格相等序号优先
					return order_id<left.order_id;
				}
				
			}
				
			else {
				if (price < left.price)
					return true;
				else if (price > left.price)
					return false;
				else {
					//价格相等序号优先
					return order_id<left.order_id;
				}
			}
				
		}
	};
	using mc_order_ptr_t  =std::shared_ptr<mc_order>;
	//订单队列
	using mc_order_list_t  = std::map<order_id_t,mc_order_ptr_t>;

	//合约某个价位
	struct mc_price_level {
		int m_price;     //真实价格*10000
		int m_total_qty; //这个价格上的总委托量
		mc_order_list_t m_order_list; //本价位上的所有订单
	};
	//同一个价位上的订单队列
	using mc_price_level_t = std::map<price_t, mc_price_level>;
	
	
	//某个合约的撮合队列
	struct mc_contract 
	{
		mc_contract(sq_match_core*core){
			m_core=core;
		};
		

		int insert_order(mc_order_ptr_t&order);
		int match_once(mc_order_ptr_t& order);
		int cancel_order(order_id_t order_id,bs_flag_t direction,int price);
        void remove_order(order_id_t order_id,bs_flag_t direction,int price);
		void dump(std::ostream&os);

		std::string 	m_contract;		//合约
		//int         	m_limt_up=0;
		//int         	m_limt_down=0;
		//合约价位表
		std::map<price_t, mc_price_level, std::greater<>> 	m_buy_price_level;
		std::map<price_t, mc_price_level, std::less<>> 		m_sell_price_level;
		sq_match_core* m_core=nullptr;
	};
    
	
	//撮合结果
	struct mc_result 
	{
		virtual ~mc_result(){};

		int mc_type; 
		contract_t contract_id;
		virtual void dump(std::ostream&os)=0;

	};
	struct mc_order_matched:public mc_result
	{
		static const  int type=1;
		contract_t 		contract;
		uint64_t 		match_id;
		int 			direction;
		order_id_t 		order_id;
		qty_t 			match_qty;
		price_t 		match_price;
		void dump(std::ostream&os)
		{
			os<<"mc_order_mached:";
			os<<"contract="<<contract.data()<<",";
			os<<"match_no="<<match_id<<",";
			os<<"direction="<<direction<<",";
			os<<"order_id="<<order_id<<",";
			os<<"match_qty="<<match_qty<<",";
			os<<"match_price="<<match_price<<"";
			os<<std::endl;
		}
	};
	
	struct mc_order_book_changed:public mc_result
	{
		static const  int type=3;
		contract_t 		contract;
		order_id_t      order_id;
		order_status_t  status;  
		void dump(std::ostream&os)
		{
			os<<"mc_order_book_changed:";
			os<<"contract="<<contract.data()<<",";
			os<<"order_id="<<order_id<<",";
			os<<"status="<<get_order_status_name(status)
			<<std::endl;
		}  
	};
	struct mc_price_level_changed:public mc_result
	{
		static const  int type=4;
		contract_t 		contract;
		bs_flag_t       direction;
		int             price;
		int             qty;
		void dump(std::ostream&os)
		{
			os<<"mc_price_level_changed:";
			os<<"contract="<<contract.data()<<",";
			os<<"direction="<<direction<<",";
			os<<"price="<<price<<",";
			os<<"qty="<<qty
			<<std::endl;
		}
			
	};
	
	
	
    /**
	 * 撮合核心 ，负责订单撮合，产生 成交回报，行情变化通知
	*/
	class sq_match_core
	{
	public:
		sq_match_core();
		~sq_match_core();

		void begin_match();
		void end_match();
		/**
		 * 增加支持的合约
		*/
		void add_contract(sq_contract* contract);

		/**
		 * @brief 下单进行撮合
		 * @param constract 合约
		 * @param sys_id    系统号
		 * @param direction 买卖
		 * @param price     委托价格
		 * @param qty
		 */
		
		int insert_order(const char*contract,order_id_t sys_id,bs_flag_t direction,int price,qty_t qty);

		int cancel_order(const char*contract,order_id_t order_id,bs_flag_t direction,int price);

		uint64_t  get_match_id() { return  ++m_match_id;};


		void on_price_level_changed(const char*contract,bs_flag_t direction, int price, int qty);
		void on_order_book_changed(const char*contract,order_id_t order_id, order_status_t status);
		void on_order_matched(const char*contract,int64_t match_id,order_id_t order_id,bs_flag_t direction,price_t match_price,qty_t match_qty);

		
		list<mc_result*> & get_match_result() {
			return m_match_results;
		}

		void dump(std::ostream& os);

	private:
		uint64_t              		m_match_id = 1;				//成交号
		std::list<mc_result*>     	m_match_results;			//每次撮合的成交结果
		std::map<std::string, mc_contract*> m_contracts; 		//所有合约表
	};
}
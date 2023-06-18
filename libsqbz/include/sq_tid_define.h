#pragma once

namespace sq
{
	//域id定义
	const int fid_sq_quot = 1;
	const int fid_req_order = 2;
	const int fid_cancel_order = 3;
	const int fid_query=4;
	const int fid_order_state=5;

	//tid定义
	const int tid_market_data = 201;    ///行情通知
	
	//const int tid_timer = 1; 			///定时器
	const int tid_place_order = 2;		///订单请求
	const int tid_order_state = 3; 		///订单状态
	const int tid_order_match =4; 		///成交通知

	const int tid_cancel_order = 6;			///撤单请求
	const int tid_trade_state = 7;			///交易状态
	const int tid_force_offset = 8; 		//强制平仓某个股票

	const int tid_login_req = 10; 			///登录请求
	const int tid_login_rsp = 11; 			///登录应答
	const int tid_contract_info = 12;  		///合约信息

    const int tid_trade_signal = 15;		//交易信号
    const int tid_clock_sync = 16;			//时钟同步包
    const int tid_arbi_market_data = 17;	//套利行情
    const int tid_arbi_order_state = 18;	//套利单的状态
    const int tid_arbi_order_match = 19;	//套利单的成交
    const int tid_level3_quot = 20;     	//level3行情
    const int tid_place_order_rsp = 21;  	//订单应答 代理服务器
    const int tid_cancel_order_rsp = 22; 	//撤单应答 代理服务器
    const int tid_modify_order = 23;     	//修改订单
    const int tid_order_modify_rsp = 24; 	///订单修改应答

	const int tid_combin_posi_req =25;      ///组合持仓应答
	const int tid_combin_posi_rsp =26;      ///组合持仓应答
	const int tid_combin_posi_trade =27;      ///组合持仓成交
   
	//查询类
	const int tid_query_req = 301; 			//查询请求（通用）
	const int tid_position_rsp = 302;		///持仓查询应答
	const int tid_query_money_rsp = 303;	///资金查询应答
	const int tid_query_order_rsp = 304;	//订单查询应答
	const int tid_query_match_rsp = 305;	//成交查询应答
	const int tid_query_rsp = 306; 			//查询应答（通用）
	const int tid_query_comb_position_rsp = 307;		///组合持仓查询应答
	//股票相关
	const int tid_risk_nty_trade_self = 100;	//风控通知 自成交
	const int tid_stock_l2_quot = 101; 			//股票L2
	const int tid_stock_match_quot = 102; 		//股票成交
	const int tid_stock_order_quot = 103; 		//股票订单
	const int tid_stock_order_queue_quot = 104; //股票订单队列
	const int tid_risk_can_trade_vol  =105;		//可成交数量
	const int tid_stock_option_quot = 111; 		//期权

//=========特征值相关==========
	const int tid_fe_quot = 401;				//特征值使用的行情
	const int tid_static_feature_idx = 402;		//静态特征值指数（期货，期权，股指，股指期权）
	const int tid_static_feature = 403;
	//股票特征值
	const int tid_stock_index_feature = 409; 	//股票指数特征值（股票）
	
    
    
}

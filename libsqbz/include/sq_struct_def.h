#pragma once
#include "sq_type_define.h"
#include "sq_tid_define.h"
#include "sq_struct_des.h"
/**
 * 结构体定义
 */
namespace sq
{
	enum enum_fid
	{
		fid_contract = 0,
		fid_rsp_msg=1,
		fid_qry_contract_req=2,
		
		

		fid_market,
		fid_product_type,
		fid_price_tick,
		fid_unit,
		fid_multiple,
		fid_max_limit_order_vol,
		fid_max_market_order_vol,
		fid_max_order_vol,
		fid_max_order_value,
		fid_max_order_count,
	};
#pragma pack(push, 1)
	struct sq_rsp_msg
	{
		static const int fid=fid_rsp_msg;
		int32_t code;
		
	};
	//品种信息
	struct sq_variety
	{
		market_id_t 	market;			 // 交易所
		variety_t variety_id;
		product_type_t 	product_type; 	// 品种类型
		price_t 		tick;			// 最小变动价位

	};
	
	// 合约信息
	struct sq_contract
	{
		static const int fid=fid_contract;
		contract_t 		contract;		 // 合约
		//variety_t 		variety;		// 品种
		market_id_t 	market;			 // 交易所
		price_t 		tick;			// 最小变动价位
		product_type_t 	product_type; 	// 品种类型
		double unit;					//报价单位
		int multiple;				   // 合约乘数
		int max_limit_order_vol;	   // 最大下单手数
		int min_limit_order_vol;	   // 最小下单手数
		int max_market_order_vol;	   // 最大市价下单手数
		int min_market_order_vol;	   // 最小市价下单手数
		int max_pos_vol;			   // 最大持仓量
		trade_date_t trade_date_begin; // 开始交易日
		trade_date_t trade_date_end;   // 最后交易日
		trade_date_t deliv_month;	   // 交割月
		trade_date_t deliv_year;	   // 交割年
		trade_date_t deliv_date_begin; // 交割开始日
		trade_date_t deliv_date_end;   // 交割结束日
		trade_date_t expiry_date;	   // 到期日
		int status;					   // 状态
		price_t benchmark_price;	   // 挂牌基准价
		
		sq_variety*  variety;
		std::string key(){
			return market.to_string()+"_"+contract.to_string();
		}

		SQ_DEF_DES_VAR(sq_contract);
	};
	//期权合约
	struct sq_opt_contract
	{
		static const int fid=fid_contract;
		contract_t 		contract;		 // 合约
		variety_t 		variety;		// 品种
		market_id_t 	market;			 // 交易所
		price_t 		tick;			// 最小变动价位
		product_type_t 	product_type; 	// 品种类型
		double unit;					//报价单位
		int multiple;				   // 合约乘数
		int max_limit_order_vol;	   // 最大下单手数
		int min_limit_order_vol;	   // 最小下单手数
		int max_market_order_vol;	   // 最大市价下单手数
		int min_market_order_vol;	   // 最小市价下单手数
		int max_pos_vol;			   // 最大持仓量
		trade_date_t trade_date_begin; // 开始交易日
		trade_date_t trade_date_end;   // 最后交易日
		trade_date_t deliv_month;	   // 交割月
		trade_date_t deliv_year;	   // 交割年
		trade_date_t deliv_date_begin; // 交割开始日
		trade_date_t deliv_date_end;   // 交割结束日
		trade_date_t expiry_date;	   // 到期日
		int 			status;					 	// 状态
		price_t 		benchmark_price;	   		// 挂牌基准价
		sq_contract* 	underliying_contract=nullptr; // 标的合约
		callpull_type_t call_put;					//看涨看跌
		price_t 		strike_price;	  			// 执行价
	};

	struct sq_margin_info
	{
		market_id_t market;
		product_id_t product;
		variety_t    variety_id;
		contract_t   contract_id;
	}
	
	// 行情信息
	struct sq_quot
	{
		tid_t tid;
		uint32_t pkg_no = 0;		  // 报文序号
		product_type_t type = f_none; // 交易类型

		market_id_t market;		   // 市场
		variety_t variety;		   // 品种类型
		uint32_t contract_idx = 0; // 合约编号
		contract_t contract;	   // 合约

		price_t price;	  // 最新价格
		qty_t match_qty;  // 成交量
		price_t turnover; // 成交额
		double interest;  // 持仓

		price_t mbl_buy_price[5];
		qty_t mbl_buy_qty[5];
		price_t mbl_sell_price[5];
		qty_t mbl_sell_qty[5];

		price_t open;		// 开盘价
		price_t close;		// 收盘价
		price_t high;		// 最高
		price_t low;		// 最低
		price_t limit_up;	// 跌板
		price_t limit_down; // 涨板

		price_t pre_close; // 昨收盘
		int multi;		   // 价格乘数因子

		// 最优行情
		inline price_t bid_price() { return mbl_buy_price[0]; };
		inline qty_t bid_qty() { return mbl_buy_qty[0]; };
		inline price_t ask_price() { return mbl_sell_price[0]; };
		inline qty_t ask_qty() { return mbl_sell_qty[0]; };

		uint64_t mstime;	// mstime
		uint64_t recv_time; // 接收时间

		trade_date_t date; // 日期:yyyymmdd
		trade_time_t time; // 时间戳hh:mm:ss.sss

		SQ_DEF_DES_VAR(sq_quot);
	};
	// 订单信息
	struct sq_order_record
	{
		user_id_t 		user_id;
		order_id_t 		sq_local_id;
		offer_id_t 		offer_id;
		contract_t 		contract;
		market_id_t 	market;
		bs_flag_t 		direction;
		offset_t 		offset;
		qty_t 			qty;				   	//委托数量
		qty_t 			match_qty;		   		//最后一次成交数量
		price_t 		match_price;	   		//最后一次成交价
		qty_t 			match_total;		   	//成交总量
		qty_t 			cancel_qty;		   		//撤单数量
		order_status_t 	status;	   				//订单状态
		order_status_t 	pre_status; 			//订单的上一个状态
		price_t 		price;			   		//委托价格
		int 			err_code;
		err_msg_t 		err_msg;
		int64_t 		insert_time;		 	//下单时间
		order_type_t 	type;			 		//订单类型
		product_type_t 	product_type; 			//产品类型
		bool 			is_finish = false;		//生命周期是否结束
		sys_order_id_t 	sys_id;		 			//交易所编号
		int64_t 		cancel_time;		 	//撤单时间

		trade_time_t quot_time; // 交易所 时间 hh:mm:ss.sss

		bool change_status(order_status_t state);
		SQ_DEF_DES_VAR(sq_order_record);
	};

	const char *get_state_name(short s);

	// 订单请求报文
	struct sq_req_order
	{
		user_id_t user_id;
		order_id_t sq_local_id;		 /// 使用者的订单编号
		market_id_t market;			 /// 市场编号
		contract_t contract;		 /// 合约
		bs_flag_t direction;		 /// 买卖方向
		offset_t offset;			 /// 开平
		price_t price;				 /// 价格
		price_t stop_price;			 /// 止损价格
		qty_t qty;					 /// 数量
		order_type_t type;			 /// 订单类型
		product_type_t product_type; /// 产品类型

		SQ_DEF_DES_VAR(sq_req_order);
	};
	// 撤单请求
	struct sq_req_cancel_order
	{
		user_id_t user_id;
		order_id_t sq_local_id;
		market_id_t market;	 /// 市场编号
		contract_t contract; /// 合约
	};
	// 订单状态变化通知,这个信息由插件产生
	struct sq_order_state_ntf
	{
		user_id_t user_id;		// user
		order_id_t sq_local_id; // 订单本地编号
		contract_t contract;	// 合约
		order_status_t status;	// 订单状态
		sys_order_id_t sys_id;	// 交易所编号
		qty_t cancel_qty;		/// 撤单数量
		bs_flag_t direction;	// 买卖方向
		offset_t offset;		// 开平方式
		int err_code;			// 错误码,下单/撤单 失败时的错误编号
		SQ_DEF_DES_VAR(sq_order_state_ntf);
	};
	// 成交通知,这个信息由插件产生
	struct sq_order_match_ntf
	{
		user_id_t user_id;
		match_id_t match_id;
		order_id_t sq_local_id;
		offer_id_t offer_id;
		contract_t contract;
		price_t match_price;
		qty_t match_qty;
		bs_flag_t direction;
		offset_t offset;
		trade_date_t date; // 成交日期
		trade_time_t time; // 成交时间
		SQ_DEF_DES_VAR(sq_order_match_ntf);
	};
	// 股票逐笔成交行情
	struct stock_match_quot
	{

		int32_t pkg_no;		 // 报文序号
		contract_t contract; // 32字节数组 产品代码 C6 / C8(如: '600000' 等)
		int64_t match_no;	 // 成交序号 / 消息记录号(从1开始，按频道连续)
		int32_t date;
		int32_t TransactTime;	 // 成交时间(HHMMSSsss)
		int32_t TradePrice;		 // 成交价格(价格单位精确到元后四位, 即: 1元 = 10000)
		int64_t TradeQty;		 // 成交数量(上海债券的数量单位为: 手)
		int64_t BidApplSeqNum;	 // 买方订单号(从 1 开始计数，0 表示无对应委托)
		int64_t OfferApplSeqNum; // 卖方订单号(从 1 开始计数，0 表示无对应委托)
		char TradeBSFlag;		 // 内外盘标志(仅适用于上证, 'B' = 外盘, 主动买, 'S' = 内盘, 主动卖, 'N' = 未知) 对于深交所, 将固定设置为 'N'(未知)
		char ExecType;			 // 成交类别(仅适用于深交所, '4' = 撤销, 'F' = 成交) 对于上证, 将固定设置为 'F'(成交
		// 由32修改为64
		uint64_t TradeMoney; // 成交金额(金额单位精确到元后四位, 即: 1元 = 10000)
		uint64_t recv_time;	 // 接收时间(毫秒)
	};
	// 股票逐笔委托行情
	struct stock_order_quot
	{
		int32_t pkg_no;		  // 报文序号
		contract_t contract;  // 产品代码 C6 / C8(如: '600000' 等)
		int32_t date;		  // 日期
		int32_t TransactTime; // 委托时间 HHMMSSsss
		int32_t Price;		  // 委托价格(价格单位精确到元后四位, 即: 1元 = 10000)
		int64_t OrderQty;	  // 委托数量
		char Side;			  // 买卖方向('1' = 买 '2' = 卖 'G' = 借入 'F' = 出借)
		char OrderType;		  // 订单类型('1' = 市价 '2' = 限价 'U' = 本方最优)
		int64_t ApplSeqNum;	  // 委托序号(从1开始，按频道连续)
		uint64_t recv_time;	  // 接收时间(毫秒)
		// 为上海逐笔委托新增
		char OrderStatus = '0'; // A - 委托订单(增加); B - 委托订单(删除)
		uint64_t BizIndex;		// 业务序号
		uint32_t channel;
		uint64_t OrderNo;
	};
	// 股票逐笔订单队列行情
	struct stock_order_queue
	{
		int32_t pkg_no;		 // 报文序号
		contract_t contract; // 产品代码 C6 / C8(如: '600000' 等)
		int32_t date;
		int32_t updateTime;		   // MdsMktDataSnapshotHeadT 中取得
		int64_t NoBidOrders;	   // 买一价位的揭示委托笔数
		int64_t NoOfferOrders;	   // 卖一价位的揭示委托笔数
		int64_t TotalVolumeTraded; // 成交总量(来自快照行情的冗余字段)
		int32_t BestBidPrice;	   // 最优申买价
		int32_t BestOfferPrice;	   // 最优申卖价
		int64_t BidOrderQty[50];   // 买一价位的委托明细(前50笔)
		int64_t OfferOrderQty[50]; // 卖一价位的委托明细(前50笔)
		uint64_t recv_time;		   // 接收时间(毫秒)
	};
	// 交易状态变化通知
	struct trade_state_changed_ntf
	{
		trade_state_t state_id;
		uint8_t type; // 1--enter 2--leave
	};
	// 持仓明细
	struct position_detail
	{
		price_t open_price = 0;
		int open_qty = 0;  // 开仓量
		int close_qty = 0; // 已经平仓量
		int direction = 0; // 持仓方向
		double profit = 0; // 每笔持仓的平仓盈亏
	};
	struct sq_contract_posi
	{
		contract_t m_contract;
		// 持仓信息
		int m_buy_opening = 0;
		int m_sell_opening = 0;
		int m_buy_closing = 0;
		int m_sell_closing = 0;

		int m_buy_posi = 0;
		int m_sell_posi = 0;
		// 买卖持仓额度
		double m_buy_turnover = 0;
		double m_sell_turnover = 0;

		// 平仓盈亏
		double m_close_profit = 0;
		//======================functions===================
		// 持仓信息管理,通过+ - 来表示总持仓方向
		// 包含了未成交的挂单
		int total_posi();
		// 所有未成交持仓
		int total_pending_posi();
		// 所有已成交的持仓
		int get_buy_posi();
		int get_sell_posi();
		// 可卖平数量
		int can_sell_close();
		// 可买平数量
		int can_buy_close();
		// 是否已经没有挂单了
		bool is_no_pending_order();
	};
    /**
	 * @brief 查询合约
	*/
	struct sq_qry_contract_req
	{
		static const int fid=fid_qry_contract_req;
		market_id_t market_id;
		contract_t  contract_id;
		
		SQ_DEF_DES_VAR(sq_qry_contract_req);
	};

	struct sq_qry_contract_info_rsp
	{

	};

#pragma pack(pop)
}
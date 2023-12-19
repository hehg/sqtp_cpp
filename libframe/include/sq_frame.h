#ifndef sq_frame_h
#define sq_frame_h
/**
 * 对交易系统的业务封装
*/
#ifdef WINDOWS
	#ifdef USE_SQ_DLL_EXPORT
	#define SQ_DLL_EXPORT __declspec(dllexport)
	#else
	#define	SQ_DLL_EXPORT __declspec(dllimport)
	#endif
#else
#define SQ_DLL_EXPORT
#endif

/**
 * @brief frame 提供一系列的c 接口
 * 这个fram 主要负责的功能包括：
 * 	1. 基于xml 配置文件加载插件
 *  2. 提供与交易平台，行情平台的 函数接口
 *  3. 内存中维护交易相关的各种数据
 *  4. 内部包含一个线程，负责将应答数据进行回调
*/

extern "C" {

	typedef void (*sq_frame_callback_func_t)(int tid, char*data, int size,void*param);
	typedef void (*sq_frame_callback_js_func_t)(int tid, char*data);

	
	//=====框架类接口=========
	SQ_DLL_EXPORT int sq_frame_open(const char*cfg_path=nullptr);
	SQ_DLL_EXPORT void sq_frame_close();
	/** 设置参数
	 * @param key 参数的名称
	 * @param val 参数的值
	 * key=callback_func	设置回调函数
	 * key=callback_param	回调函数参数
	 * key=config_file		配置文件路径
	*/
	SQ_DLL_EXPORT int sq_frame_set_option(const char*key,void*val);

    //=====行情相关接口=========
	//订阅行情
	//=====交易相关接口=========

	/**
	 * @brief 买开
	 * @param local_id 本地订单编号，不能重复
	 * @param contract 合约编号
	 * @param price    订单价格
	 * @param qty 	   订单数量
	 * @return 0 成功  否则 失败
	*/
	SQ_DLL_EXPORT int sq_frame_buy_open(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int sq_frame_sell_open(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int sq_frame_buy_close(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int sq_frame_sell_close(int local_id,const char*contract,double price,int qty);

	SQ_DLL_EXPORT int sq_frame_place_order(int local_id,
								  const char *market,
								  const char *contract,
								  int direction, int offset,
								  double price,
								  int qty,
								  int order_type, int product_type);

	SQ_DLL_EXPORT int sq_frame_cancel_order(int local_id);
 	//撤销某个方向的所有订单
    SQ_DLL_EXPORT void sq_frame_cancel_all_order(int direction);

	//====查询接口===

	/**
	 * @brief 查询合约
	 * @param market  市场编号，不指定表示所有市场
	 * @param contract 合约id，空表示所有合约
	 * 
	*/
	SQ_DLL_EXPORT int sq_frame_qry_contract(const char*market,const char*contract);

	//=====其它接口===============
	SQ_DLL_EXPORT const char* sq_frame_get_error_msg(int err_no);
	SQ_DLL_EXPORT const char* sq_frame_get_cur_time();
}
#endif 
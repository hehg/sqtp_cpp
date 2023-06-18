#ifndef sq_frame_h
#define sq_frame_h

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
*/

extern "C" {

	typedef void (*sq_frame_callback_func_t)(int tid, char*data, int size,void*param);
	typedef void (*sq_frame_callback_js_func_t)(int tid, char*data);

	
	//=====框架类接口=========
	SQ_DLL_EXPORT int sq_frame_open();
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
	SQ_DLL_EXPORT int buy_open(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int sell_open(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int buy_close(int local_id,const char*contract,double price,int qty);
	SQ_DLL_EXPORT int sell_close(int local_id,const char*contract,double price,int qty);

	SQ_DLL_EXPORT int place_order(int local_id,
								  const char *market,
								  const char *contract,
								  int direction, int offset,
								  double price,
								  int qty,
								  int order_type, int product_type);

	SQ_DLL_EXPORT int cancel_order(int local_id);
 	//撤销某个方向的所有订单
    SQ_DLL_EXPORT void cancel_all_order(int direction);

	//=====其它接口===============
	SQ_DLL_EXPORT const char* sq_frame_get_error_msg(int err_no);
	SQ_DLL_EXPORT const char* sq_frame_get_cur_time();
}
#endif 
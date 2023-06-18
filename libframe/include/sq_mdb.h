#ifndef SQ_MDB_H
#define SQ_MDB_H

#ifdef WINDOWS
#ifdef USE_SQ_DLL_EXPORT
#define SQ_DLL_EXPORT __declspec(dllexport)
#else
#define SQ_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define SQ_DLL_EXPORT
#endif

#include "sq_struct_def.h"
using namespace sq;

/**
 * @brief 与内存数据相关的查询接口，都在此文件中提供
*/
extern "C"
{
	SQ_DLL_EXPORT int sq_mdb_open();
	SQ_DLL_EXPORT int sq_mdb_close();

	// 获取表
	SQ_DLL_EXPORT void *get_table(const char *name);
	// 处理业务数据
	SQ_DLL_EXPORT int sq_mdb_put(int tid, char *data, int size);

	// 参数设置
	SQ_DLL_EXPORT int sq_mdb_set_option(const char *key, void *val);
	SQ_DLL_EXPORT int sq_mdb_get_option(const char *key, void *val);

	// 订单表
	SQ_DLL_EXPORT sq::sq_order_record *sq_mdb_get_order_by_id(int local_id);
	SQ_DLL_EXPORT sq::sq_order_record *sq_mdb_get_order_by_index(int index);
    SQ_DLL_EXPORT int sq_mdb_order_tb_size();
	// 持仓表
	SQ_DLL_EXPORT int sq_mdb_get_buy_posi(const char*contract);
	SQ_DLL_EXPORT int sq_mdb_get_sell_posi(const char*contract);
	SQ_DLL_EXPORT double sq_mdb_get_profit();
	SQ_DLL_EXPORT int sq_mdb_get_posi_table_size();
	SQ_DLL_EXPORT sq_contract_posi* sq_mdb_get_posi_record(int index);

	// 持仓明细
	

	// 行情表

    SQ_DLL_EXPORT sq::sq_quot* sq_mdb_get_quot(const char*contract);
	SQ_DLL_EXPORT void sq_mdb_dump();
}
#endif
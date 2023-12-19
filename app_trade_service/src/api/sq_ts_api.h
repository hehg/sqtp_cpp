#pragma once

#ifdef WINDOWS
	#ifdef USE_SQ_DLL_EXPORT
	#define SQ_DLL_EXPORT __declspec(dllexport)
	#else
	#define	SQ_DLL_EXPORT __declspec(dllimport)
	#endif
#else
#define SQ_DLL_EXPORT
#endif
#include "sq_struct_def.h"
using namespace sq;
class sq_ts_api_rsp
{
public:
    virtual void on_qry_contract_list_rsp(sq_rsp_msg*rsp_msg,sq_contract*data) = 0;
};


extern "C" {
    int sq_ts_api_connect(const char* address);
    int sq_ts_api_disconnect();
    void sq_ts_api_set_rsp(sq_ts_api_rsp* rsp);
    /**
     * 查询合约列表
    */
    int sq_ts_api_qry_contract_list(const char* market,const char*contract);
}
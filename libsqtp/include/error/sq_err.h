/**
Copyright (c) 2003-2015 
@brief 平台错误编码,以及与错误相关信息的定义


@author 
*/
#pragma once
#include "sq_define.h"
namespace sq
{
    //sqtp
    const int32_t ok = 0;
    const int32_t err_fail = 1;
    const int32_t err_time_out = 2;
    const int32_t err_not_exist = 3;
    const int32_t err_call_api = 4;
    const int32_t err_not_support = 5;
    const int32_t err_repeat = 6;
	const int32_t err_invalid_package=7;
    const int32_t err_invalid_param=8;
    const int32_t err_again=9;
    const int32_t err_sql_exec=10;
    const int32_t err_db_connect=11;

    //offer
    const int32_t err_no_exit_order = 301;
    const int32_t err_no_login = 302;
    const int32_t err_no_enough_posi=303;
	const int32_t err_repeat_order_id = 304;
    //异常
    void sq_abort(const char *errmsg_);

    const char* get_last_error_info();

   
    const char *get_err_name(int code);
    
    void add_error(int code, const char *msg);
    
    void add_errors(std::map<int, const char *> errs);
    
}
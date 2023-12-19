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
    const int32_t err_db_connect=11;                    //数据库链接失败      
    const int32_t err_net_connect=12;                   //网络链接失败
    const int32_t err_invalid_net_address=13;           //无效的网络地址
    const int32_t err_load_data=14;
    const int32_t err_no_data=15;

    //offer
    const int32_t err_no_exit_order = 301;
    const int32_t err_no_login = 302;
    const int32_t err_no_enough_posi=303;
	const int32_t err_repeat_order_id = 304;


    //网关错误
	const int invalid_client_id = 1001;
	const int invalid_local_id = 1002;
	const int invalid_contract = 1003;
	const int invalid_direction_flag = 1004;
	const int invalid_offset_flag = 1005;
	const int invalid_order_qty = 1006;
	const int invalid_order_price = 1007;
	const int invalid_order_type = 1008;
	const int invalid_passwd = 1009;

	//业务错误
	const int client_passwd_err = 2001; //密码错误
	const int client_no_right = 2002;  //无权限
	const int client_no_exist = 2003; //不存在的用户
	const int client_repead_id = 2004; //重复的用户
	const int no_exist_client = 2005;//不存在的客户
	const int client_repead_login = 2006; //重复登录


	const int money_not_enough = 2010;//资金不够
	const int posi_not_enouch = 2011;//持仓不够
	const int no_exist_contract = 2012; //不存在的合约
	const int no_exist_order = 2013; //不存在的订单
	const int cancel_order_err = 2014;//撤单失败
	const int repeat_local_id = 2015; //重复本地报单号
	const int no_support_contract = 2016; //不支持的合约
	const int no_exist_price = 2017; //不存在的价格

    
    //异常
    void sq_abort(const char *errmsg_);

    const char* get_last_error_info();

   
    const char *get_err_name(int code);
    
    void add_error(int code, const char *msg);
    
    void add_errors(std::map<int, const char *> errs);
    
}
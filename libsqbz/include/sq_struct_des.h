#pragma once

#include "pack/struct_reflect.h"



// 构造一个描述类
namespace sq
{


    class sq_contract_desc : public struct_reflect
    {
    public:
        sq_contract_desc();
    };
    class sq_quot_desc : public struct_reflect
    {
        public:
        sq_quot_desc();
    };
    class sq_req_order_desc : public struct_reflect
    {
        public:
        sq_req_order_desc();
    };
    class sq_order_record_desc : public struct_reflect
    {
        public:
        sq_order_record_desc();
    };
    class sq_order_match_ntf_desc : public struct_reflect
    {
        public:
        sq_order_match_ntf_desc();  
    };
    class sq_order_state_ntf_desc : public struct_reflect
    {
        public:
        sq_order_state_ntf_desc();
    };
    class sq_qry_contract_req_desc:public struct_reflect
    {
        public:
        sq_qry_contract_req_desc();
    };

}
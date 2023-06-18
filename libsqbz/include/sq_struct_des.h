#pragma once

#include "pack/struct_reflect.h"

#define SQ_DES_DECLARE(class_name) class class_name##_desc:public struct_reflect {public:class_name##_desc();};
namespace sq
{
    SQ_DES_DECLARE(sq_quot)
    SQ_DES_DECLARE(sq_req_order)
    SQ_DES_DECLARE(sq_order_record)
    SQ_DES_DECLARE(sq_order_match_ntf)
    SQ_DES_DECLARE(sq_order_state_ntf)

}
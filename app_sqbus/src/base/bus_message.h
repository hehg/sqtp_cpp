#pragma once
#include "pack/sq_package.h"
namespace bus
{
    enum bus_tid{
        bus_tid_sub_topic=1000,          //订阅请求
        bus_tid_req_result=1001,        //请求应答
        bus_tid_pub_topic=1002,         //发布某个主题的消息
    };
    struct bus_subcribe_req
    {
        const static uint16_t fid=1;
        uint32_t topic;
    };

    struct bus_req_result
    {
        const static uint16_t fid=2;
        uint32_t topic;
        int result;
    };
}
#pragma once
#include "sq_struct_def.h"
namespace sq
{
#pragma pack(push, 1)
    // 股票静态特征值
    struct stock_fe_result
    {
        int tid;
        int stock_opened;
        double idxPrice;
        double rtVipInflow;
        double rtVipActiveInflow;
        double momentumAmt;
        int64_t volume;
        uint64_t localTimeStamp;
        int number;
        int time;
        double rtTurnover;

        double ABDPDiffF = 0;
        double ABDPDiffM = 0;
        double ABVDiffF = 0;
        double ABVDiffM = 0;

        double active_vol = 0;
    };

#pragma pack(pop)

}
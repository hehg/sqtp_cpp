#pragma once

namespace sq_plug
{
    class stock_fe_base
    {
        public:
        virtual void add_stock(int idx,double w, double s)=0;
		virtual void put(int tid,char*data,int size)=0;
		virtual void calc_result(void *result)=0;
    };
}
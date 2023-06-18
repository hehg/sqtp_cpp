#pragma once
#include "sq_define.h"
#include "sq_store.h"
namespace sq
{
    /** 数据流读取器
    */
    class sq_store_reader
    {
    public:
        sq_store_reader() { };
        ~sq_store_reader() {};

        //数据流有新数据时会通过这个接口回调给reader
        virtual void on_store_data(int64_t id, void*data, int size) {}
        void set_next_read_id(int64_t no) { m_read_no = no; }
    protected:
        int64_t m_read_no = 0;//当前读取序号
    };
}
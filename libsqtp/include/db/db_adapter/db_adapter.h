#pragma once
#include "sq_types.h"
namespace sq
{
    // 绑定信息
    struct db_bind_info
    {
        field_type_t type;
        unsigned short len;
        union
        {
            int int_data;
            int64_t int64_data;
            double double_data;
            const char* text_data;
        } bind_data;
    };

    /**
     * 查询结果的一行
     */
    struct db_result_row
    {
        std::vector<std::string> fields_;

        double      as_double(int idx)const { return atof(fields_[idx].c_str()); }
        int64_t     as_int64(int idx) const { return atoll(fields_[idx].c_str()); }
        int         as_int(int idx) const { return atoi(fields_[idx].c_str()); }
        const std::string &as_string(int idx)const { return fields_[idx]; }
    };
    // 查询结果集合
    struct db_result_table
    {
        int  effect_rows_=0;
        std::vector<std::string>   col_names_;  //每个列的名字
        std::vector<db_result_row> rows_;

        db_result_row *next()
        {
            if (cur_record_ < rows_.size())
            {
                return &(rows_[cur_record_++]);
            }
            return nullptr;
        }

        void clear()
        {
            rows_.clear();
            cur_record_ = 0;
        }

        unsigned int cur_record_ = 0;
    };
    using db_result_func_t=std::function<void(const db_result_table &)>;
    //using db_exception_fun_t=std::function<void(const std::exception_ptr &)

}
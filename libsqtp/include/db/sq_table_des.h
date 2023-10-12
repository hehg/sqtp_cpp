/**
 * 描述表结构的数据定义
*/
#pragma once
#include <string>
#include <vector>
namespace sq
{
    /**
     * @brief 数据库表的描述信息
    */
    struct sq_table_col_desc{
        sq_table_col_desc(const char*name,
        const char*type,bool is_key=false,bool not_null=false,int array_size=0){
             m_name = name;
             m_type= type;
             m_is_not_null=not_null;
             m_is_primary_key=is_key;
             m_array_size=array_size;
        }
        std::string m_name;             //名字
        std::string m_type;             //类型
        bool m_is_array=false;
        int  m_array_size=0;                   //数组大小
        bool m_is_primary_key=false;          //是否为主键
        bool m_is_not_null=false;             //是否可null
    };
    struct sq_table_desc{
        std::string                     m_table_name;
        std::vector<sq_table_col_desc>  m_columns;
        template<typename T>
        int get_fild_value(int index);
    };
}
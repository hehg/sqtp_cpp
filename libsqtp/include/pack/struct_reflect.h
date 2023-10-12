#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "sq_types.h"
#include "log/sq_logger.h"

#define ADD_FD(s, t, f)         add_fld_des(#f, t, sizeof(s::f), offsetof(s, f));
#define ADD_FD_BOOL(s, f)       add_fld_des(#f, field_type_t::type_bool, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT8(s, f)       add_fld_des(#f, field_type_t::type_int8, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT8(s, f)      add_fld_des(#f, field_type_t::type_uint8, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT16(s, f)     add_fld_des(#f, field_type_t::type_uint16, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT32(s, f)      add_fld_des(#f, field_type_t::type_int32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT32(s, f)     add_fld_des(#f, field_type_t::type_uint32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT64(s, f)      add_fld_des(#f, field_type_t::type_int64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT64(s, f)     add_fld_des(#f, field_type_t::type_uint64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT32(s, f)    add_fld_des(#f, field_type_t::type_float32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT64(s, f)    add_fld_des(#f, field_type_t::type_float64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_STR(s, f)        add_fld_des(#f, field_type_t::type_char_array, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT64_ARRAY(s, f, c)   add_fld_des(#f, field_type_t::type_float64_array, sizeof(s::f), offsetof(s, f), c);
#define ADD_FD_INT_ARRAY(s, f, c)       add_fld_des(#f, field_type_t::type_int32_array, sizeof(s::f), offsetof(s, f), c);
namespace sq
{
   
    //结构体每个字段的属性
    struct field_describe_t
    {
        field_describe_t()=default;

        field_describe_t(const char *name, 
        const char*type_name, 
        int item_size,
        size_t size, 
        size_t offset)
            : m_name(name), m_type_name(type_name), m_size(size), m_offset(offset)
        {
            if(item_size!=size){
                m_array_item_size=item_size;
                m_is_array=true;
                m_array_item_count=size/item_size;
            }
            //检查类型
            
            if(m_type_name=="uint8_t"){
                m_type=field_type_t::type_uint8;
            }
            else if (m_type_name == "char")
            {
                m_type = field_type_t::type_char;
                if(m_is_array){
                    m_type=field_type_t::type_char_array;
                }
            }
            else if (m_type_name == "int8_t")
            {
                m_type = field_type_t::type_int8;
            }
            else if(m_type_name=="int16_t"){
                m_type=field_type_t::type_int16;
            }
            else if(m_type_name=="uint16_t"){
                m_type=field_type_t::type_uint16;
            }
            else if(m_type_name=="int"||m_type_name=="int32_t"){
                m_type=field_type_t::type_int32;
                if (m_is_array)
                {
                    m_type=field_type_t::type_int32_array;
                }
                
            }
            else if(m_type_name=="uint32_t"){
                m_type=field_type_t::type_uint32;
                if (m_is_array)
                {
                    m_type=field_type_t::type_int32_array;
                }
            }
             else if(m_type_name=="int64"){
                m_type=field_type_t::type_int64;

            }
            else if(m_type_name=="uint64"){
                m_type=field_type_t::type_uint64;
            }
            else if(m_type_name=="bool"){
                m_type=field_type_t::type_bool;
            }
             else if(m_type_name=="float"){
                m_type=field_type_t::type_float32;
            }
             else if(m_type_name=="double"){
                m_type=field_type_t::type_float64;
                if (m_is_array)
                {
                    m_type=field_type_t::type_float64_array;
                }
            }
            else{
                sq_panic("not support type_name=%s\n",m_type_name.c_str());
            }

            if (m_type > field_type_t::type_number_begin &&
                m_type < field_type_t::type_number_end)
            {
                m_class_type = field_class_t::class_integer;
            }
            else if (m_type > field_type_t::type_text_begin &&
                m_type < field_type_t::type_text_end)
            {
                m_class_type = field_class_t::class_text;
            }
            else if (m_type > field_type_t::type_real_begin &&
                m_type < field_type_t::type_real_end)
            {
                m_class_type = field_class_t::class_real;
            }
            else{
                sq_panic("not support type_name=%s\n",m_type_name.c_str());
            }
        }
        std::string     m_name;     //名称
        field_type_t    m_type;     //类型
        std::string     m_type_name;
        //std::string     m_type_class; //integer,text,real
        uint16_t        m_size;     //大小
        uint16_t        m_offset;   //偏移
        field_class_t   m_class_type=field_class_t::class_none;

        bool            m_is_array=false;
        int             m_array_item_size=0;
        int             m_array_item_count=0;            //当为数组时，数组的大小
        bool            m_is_primary_key=false;  //是否为主键
    };
    /**
     * 记录数据结构的元信息
    */
    struct struct_reflect
    {
        std::string m_name; //结构体的名字
        int         m_size; //结构体的大小

        std::vector<field_describe_t> m_fields;
        /**
         * @brief 添加一个域信息
        */
        void add_fld_des(const char *name, 
                        field_type_t type,
                         uint16_t size, 
                         uint16_t offset, 
                         int array_size = 0)
        {
            field_describe_t f;//{name,type,size,offset,array_size};
            f.m_name = name;
            f.m_type=type;
            f.m_offset=offset;
            f.m_array_item_count=array_size;
            //f.m_array_item_size
            m_size      += size;
            m_fields.emplace_back(f);
        }

        void to_string(void *obj, std::ostream &out = std::cout);
        void to_csv(void *obj, std::ostream &out = std::cout);
        void to_json(void*obj,std::ostream &out=std::cout);

        std::tuple<field_type_t,char*>  get_field_val(void*obj,int index);

    };
    extern  std::unordered_map<std::string, struct_reflect *> s_desc_map;
}



#define SQ_DES_DECLARE(class_name) class class_name##_desc:public struct_reflect {public:class_name##_desc();};

#define SQ_DES_DEFINE_BEGIN(class_name)  class_name##_desc class_name::s_##class_name##_desc; \
    class_name##_desc::class_name##_desc()

//#define SQ_DES_DEFINE_END(class_name)
#define SQ_DEF_DES_VAR(class_name) static class_name##_desc s_##class_name##_desc; \
    void to_string(std::ostream&of){\
        s_##class_name##_desc.to_string(this,of);\
    }\
    void  to_csv(std::ostream&of){\
        s_##class_name##_desc.to_csv(this,of);\
    }\
    void to_json(std::ostream&of){\
        s_##class_name##_desc.to_json(this,of);\
    }
    
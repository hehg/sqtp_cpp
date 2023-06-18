#pragma once
#include <string>
#include <vector>
#include <iostream>

#define ADD_FD(s, t, f) add_fld_des(#f, t, sizeof(s::f), offsetof(s, f));
#define ADD_FD_BOOL(s, f) add_fld_des(#f, field_type_t::type_bool, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT8(s, f) add_fld_des(#f, field_type_t::type_int8, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT8(s, f) add_fld_des(#f, field_type_t::type_uint8, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT16(s, f) add_fld_des(#f, field_type_t::type_uint16, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT32(s, f) add_fld_des(#f, field_type_t::type_int32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT32(s, f) add_fld_des(#f, field_type_t::type_uint32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_INT64(s, f) add_fld_des(#f, field_type_t::type_int64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_UINT64(s, f) add_fld_des(#f, field_type_t::type_uint64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT32(s, f) add_fld_des(#f, field_type_t::type_float32, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT64(s, f) add_fld_des(#f, field_type_t::type_float64, sizeof(s::f), offsetof(s, f));
#define ADD_FD_STR(s, f) add_fld_des(#f, field_type_t::type_fix_str, sizeof(s::f), offsetof(s, f));
#define ADD_FD_FLOAT64_ARRAY(s, f, c) add_fld_des(#f, field_type_t::type_float64_array, sizeof(s::f), offsetof(s, f), c);
#define ADD_FD_INT_ARRAY(s, f, c) add_fld_des(#f, field_type_t::type_int_array, sizeof(s::f), offsetof(s, f), c);
namespace sq
{
    enum class field_type_t : uint8_t
    {
        type_uint8 = 1,
        type_int8,
        type_uint16,
        type_int16,
        type_uint32,
        type_int32,
        type_uint64,
        type_int64,
        type_float32,
        type_float64,
        type_fix_str,
        type_float64_array,
        type_int_array,
        type_bool,
    };
    struct field_describe_t
    {
        std::string name;   //名称
        field_type_t type;  //类型
        uint16_t item_size; //大小
        uint16_t offset;    //偏移
        uint16_t array_size;
    };

    struct struct_reflect
    {
        std::string name;
        int m_size;

        std::vector<field_describe_t> m_fields;

        void add_fld_des(const char *name, field_type_t type,
                         uint16_t item_size, uint16_t struct_offset, int array_size = 0)
        {
            field_describe_t f;
            f.name = name;
            f.item_size = item_size;
            f.type = type;
            f.offset = struct_offset;
            f.array_size = array_size;
            m_size += item_size;
            m_fields.push_back(f);
        }

        void to_string(void *obj, std::ostream &out = std::cout);
        void to_csv(void *obj, std::ostream &out = std::cout);
        void to_json(void*obj,std::ostream &out=std::cout);
    };
}
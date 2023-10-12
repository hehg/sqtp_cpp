#include "pack/struct_reflect.h"
#include <assert.h>
#include <sstream>
#include "text/json.hpp"
#include "fmt/format.h"
namespace sq
{
    std::unordered_map<std::string, struct_reflect *> s_desc_map;

    std::tuple<field_type_t,char*>  struct_reflect::get_field_val(void*obj,int index)
    {
        char *self = (char *)obj;
        field_describe_t *it = &m_fields[index];
        std::tuple<field_type_t,char*> ret = std::make_tuple(it->m_type,self+it->m_offset);
        return ret;
    }
    
    void struct_reflect::to_string(void *obj, std::ostream &out)
    {
        char *self = (char *)obj;
        using namespace std;
        for (auto it = m_fields.begin(); it != m_fields.end(); ++it)
        {
            std::string val;
            switch (it->m_type)
            {
            case field_type_t::type_bool:
                val=fmt::to_string((bool)*(bool *)(self + it->m_offset));
                break;
            case field_type_t::type_int8:
               val=fmt::to_string((int)*(int8_t *)(self + it->m_offset));
                break;
            case field_type_t::type_uint8:
                val=fmt::to_string((unsigned int)*(uint8_t *)(self + it->m_offset));
                break;
            case field_type_t::type_int16:
                val=fmt::to_string( *(int16_t *)(self + it->m_offset));
                break;
            case field_type_t::type_uint16:
                val=fmt::to_string(*(uint16_t *)(self + it->m_offset));
                break;
            case field_type_t::type_int32:
                val=fmt::to_string( *(int32_t *)(self + it->m_offset));
                break;
            case field_type_t::type_uint32:
                val=fmt::to_string(*(uint32_t *)(self + it->m_offset));
                break;
            case field_type_t::type_int64:
                val=fmt::to_string( *(int64_t *)(self + it->m_offset));
                break;
            case field_type_t::type_uint64:
                val=fmt::to_string(*(uint64_t *)(self + it->m_offset));
                break;
            case field_type_t::type_float32:
               val=fmt::to_string( *(float *)(self + it->m_offset));
                break;
            case field_type_t::type_float64:
                val=fmt::to_string(*(double *)(self + it->m_offset));
                break;
            case field_type_t::type_char_array:
                val=fmt::to_string((char *)(self + it->m_offset));
                break;
            case field_type_t::type_int32_array:
            {
                std::stringstream ss;
                ss<<"[";
                for (int i = 0; i < it->m_array_item_count; i++)
                {
                    if (i < it->m_array_item_count - 1)
                    {
                        ss << *((int *)(self + it->m_offset + i * sizeof(int))) << ",";
                    }
                    else
                    {
                        ss << *(int *)(self + it->m_offset + i * sizeof(int));
                    }
                }
                ss << "]";
                val=ss.str();
            }
            break;
            case field_type_t::type_float64_array:
            {
               std::stringstream ss;
                ss<<"[";
                for (int i = 0; i < it->m_array_item_count; i++)
                {
                    if (i < it->m_array_item_count - 1)
                    {
                        ss << *((double *)(self + it->m_offset + i * sizeof(double))) << ",";
                    }
                    else
                    {
                        ss << *(double *)(self + it->m_offset + i * sizeof(double));
                    }
                }
                ss << "]";
                val=ss.str();
            }
            break;
            default:
                sq_panic("not support field_type=%d\n",(int)it->m_type);
                break;
            }
            out<<it->m_name<<"="<<val;
            if (it!=m_fields.end()-1)
            {
                out << ",";
            }
        }
    }

    void struct_reflect::to_csv(void *obj, std::ostream &out)
    {
        char *self = (char *)obj;
        using namespace std;
        const char *sep = ",";
        // const char* end
        for (int i = 0; i < m_fields.size(); i++)
        {
            field_describe_t *it = &m_fields[i];
            switch (it->m_type)
            {
            case field_type_t::type_bool:
                out << (int)(*(int8_t *)(self + it->m_offset));
                break;
            case field_type_t::type_int8:
                out << (int)(*(int8_t *)(self + it->m_offset));
                break;
            case field_type_t::type_uint8:
                out << (int)(*(uint8_t *)(self + it->m_offset));
                break;
            case field_type_t::type_int16:
                out << *(int16_t *)(self + it->m_offset);
                break;
            case field_type_t::type_uint16:
                out << *(uint16_t *)(self + it->m_offset);
                break;
            case field_type_t::type_int32:
                out << *(int32_t *)(self + it->m_offset);
                break;
            case field_type_t::type_uint32:
                out << *(uint32_t *)(self + it->m_offset);
                break;
            case field_type_t::type_int64:
                out << *(int64_t *)(self + it->m_offset);
                break;
            case field_type_t::type_uint64:
                out << *(uint64_t *)(self + it->m_offset);
                break;
            case field_type_t::type_float32:
                out << *(float *)(self + it->m_offset);
                break;
            case field_type_t::type_float64:
                out << *(double *)(self + it->m_offset);
                break;
            case field_type_t::type_char_array:
                out << (char *)(self + it->m_offset);
                break;
            case field_type_t::type_int32_array:
            {
                for (int i = 0; i < it->m_array_item_count; i++)
                {
                    if (i < it->m_array_item_count - 1)
                    {
                        out << *((int *)(self + it->m_offset + i * sizeof(int))) << ",";
                    }
                    else
                    {
                        out << *(int *)(self + it->m_offset + i * sizeof(int));
                    }
                }
            }
            break;
            case field_type_t::type_float64_array:
            {

                for (int i = 0; i < it->m_array_item_count; i++)
                {
                    if (i < it->m_array_item_count - 1)
                    {
                        out << *((double *)(self + it->m_offset + i * sizeof(double))) << ",";
                    }
                    else
                    {
                        out << *(double *)(self + it->m_offset + i * sizeof(double));
                    }
                }
            }
            break;
            default:
                assert(false);
                break;
            }

            if (i != m_fields.size() - 1)
            {
                out << ",";
            }
        }
    }
    void struct_reflect::to_json(void *obj, std::ostream &out)
    {
        char *self = (char *)obj;
        using namespace std;
        using namespace nlohmann;
        nlohmann::json js;
        try
        {
            for (int i = 0; i < m_fields.size(); i++)
            {
                field_describe_t *it = &m_fields[i];
                switch (it->m_type)
                {
                case field_type_t::type_bool:
                    js[it->m_name] = (bool)*(bool *)(self + it->m_offset);
                    break;
                case field_type_t::type_int8:
                    js[it->m_name] = (int)*(int8_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_uint8:
                    js[it->m_name] = (unsigned int)*(uint8_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_int16:
                    js[it->m_name] = *(int16_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_uint16:
                    js[it->m_name] = *(uint16_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_int32:
                    js[it->m_name] = *(int32_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_uint32:
                    js[it->m_name] = *(uint32_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_int64:
                    js[it->m_name] = *(int64_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_uint64:
                    js[it->m_name] = *(uint64_t *)(self + it->m_offset);
                    break;
                case field_type_t::type_float32:
                    js[it->m_name] = *(float *)(self + it->m_offset);
                    break;
                case field_type_t::type_float64:
                    js[it->m_name] = *(double *)(self + it->m_offset);
                    break;
                case field_type_t::type_char_array:
                    js[it->m_name] = (char *)(self + it->m_offset);
                    break;
                case field_type_t::type_int32_array:
                {
                    std::vector<int> vec;

                    for (int i = 0; i < it->m_array_item_count; i++)
                    {
                        if (i < it->m_array_item_count - 1)
                        {
                            vec .push_back( *((int *)(self + it->m_offset + i * sizeof(int))));
                        }
                        else
                        {
                            vec .push_back( *(int *)(self + it->m_offset + i * sizeof(int)));
                        }
                    }
                    js[it->m_name] = vec;
                }
                break;
                case field_type_t::type_float64_array:
                {
                    std::vector<double> vec;
                    for (int i = 0; i < it->m_array_item_count; i++)
                    {
                        if (i < it->m_array_item_count - 1)
                        {
                           vec .push_back( *((double *)(self + it->m_offset + i * sizeof(double))));
                        }
                        else
                        {
                            vec .push_back( *(double *)(self + it->m_offset + i * sizeof(double)));
                        }
                    }
                    js[it->m_name] = vec;
                }
                break;
                default:
                    assert(false);
                    break;
                }
            }
        }
        catch (...)
        {
            printf("to json error\n");
        }

        out << js.dump();
    }
}
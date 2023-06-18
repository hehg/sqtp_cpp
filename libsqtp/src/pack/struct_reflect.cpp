#include "pack/struct_reflect.h"
#include <assert.h>
#include "text/json.hpp"
namespace sq
{
    void struct_reflect::to_string(void *obj, std::ostream &out)
    {
        char *self = (char *)obj;
        using namespace std;
        const char *sep = ",";
        // const char* end
        for (int i = 0; i < m_fields.size(); i++)
        {
            field_describe_t *it = &m_fields[i];
            switch (it->type)
            {
            case field_type_t::type_bool:
                out << it->name << "=" << (bool)*(bool *)(self + it->offset);
                break;
            case field_type_t::type_int8:
                out << it->name << "=" << (int)*(int8_t *)(self + it->offset);
                break;
            case field_type_t::type_uint8:
                out << it->name << "=" << (unsigned int)*(uint8_t *)(self + it->offset);
                break;
            case field_type_t::type_int16:
                out << it->name << "=" << *(int16_t *)(self + it->offset);
                break;
            case field_type_t::type_uint16:
                out << it->name << "=" << *(uint16_t *)(self + it->offset);
                break;
            case field_type_t::type_int32:
                out << it->name << "=" << *(int32_t *)(self + it->offset);
                break;
            case field_type_t::type_uint32:
                out << it->name << "=" << *(uint32_t *)(self + it->offset);
                break;
            case field_type_t::type_int64:
                out << it->name << "=" << *(int64_t *)(self + it->offset);
                break;
            case field_type_t::type_uint64:
                out << it->name << "=" << *(uint64_t *)(self + it->offset);
                break;
            case field_type_t::type_float32:
                out << it->name << "=" << *(float *)(self + it->offset);
                break;
            case field_type_t::type_float64:
                out << it->name << "=" << *(double *)(self + it->offset);
                break;
            case field_type_t::type_fix_str:
                out << it->name << "=" << (char *)(self + it->offset);
                break;
            case field_type_t::type_int_array:
            {
                out << it->name << "=[";
                for (int i = 0; i < it->array_size; i++)
                {
                    if (i < it->array_size - 1)
                    {
                        out << *((int *)(self + it->offset + i * sizeof(int))) << ",";
                    }
                    else
                    {
                        out << *(int *)(self + it->offset + i * sizeof(int));
                    }
                }
                out << "]";
            }
            break;
            case field_type_t::type_float64_array:
            {
                out << it->name << "=[";
                for (int i = 0; i < it->array_size; i++)
                {
                    if (i < it->array_size - 1)
                    {
                        out << *((double *)(self + it->offset + i * sizeof(double))) << ",";
                    }
                    else
                    {
                        out << *(double *)(self + it->offset + i * sizeof(double));
                    }
                }
                out << "]";
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

    void struct_reflect::to_csv(void *obj, std::ostream &out)
    {
        char *self = (char *)obj;
        using namespace std;
        const char *sep = ",";
        // const char* end
        for (int i = 0; i < m_fields.size(); i++)
        {
            field_describe_t *it = &m_fields[i];
            switch (it->type)
            {
            case field_type_t::type_bool:
                out << (int)(*(int8_t *)(self + it->offset));
                break;
            case field_type_t::type_int8:
                out << (int)(*(int8_t *)(self + it->offset));
                break;
            case field_type_t::type_uint8:
                out << (int)(*(uint8_t *)(self + it->offset));
                break;
            case field_type_t::type_int16:
                out << *(int16_t *)(self + it->offset);
                break;
            case field_type_t::type_uint16:
                out << *(uint16_t *)(self + it->offset);
                break;
            case field_type_t::type_int32:
                out << *(int32_t *)(self + it->offset);
                break;
            case field_type_t::type_uint32:
                out << *(uint32_t *)(self + it->offset);
                break;
            case field_type_t::type_int64:
                out << *(int64_t *)(self + it->offset);
                break;
            case field_type_t::type_uint64:
                out << *(uint64_t *)(self + it->offset);
                break;
            case field_type_t::type_float32:
                out << *(float *)(self + it->offset);
                break;
            case field_type_t::type_float64:
                out << *(double *)(self + it->offset);
                break;
            case field_type_t::type_fix_str:
                out << (char *)(self + it->offset);
                break;
            case field_type_t::type_int_array:
            {
                for (int i = 0; i < it->array_size; i++)
                {
                    if (i < it->array_size - 1)
                    {
                        out << *((int *)(self + it->offset + i * sizeof(int))) << ",";
                    }
                    else
                    {
                        out << *(int *)(self + it->offset + i * sizeof(int));
                    }
                }
            }
            break;
            case field_type_t::type_float64_array:
            {

                for (int i = 0; i < it->array_size; i++)
                {
                    if (i < it->array_size - 1)
                    {
                        out << *((double *)(self + it->offset + i * sizeof(double))) << ",";
                    }
                    else
                    {
                        out << *(double *)(self + it->offset + i * sizeof(double));
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
                switch (it->type)
                {
                case field_type_t::type_bool:
                    js[it->name] = (bool)*(bool *)(self + it->offset);
                    break;
                case field_type_t::type_int8:
                    js[it->name] = (int)*(int8_t *)(self + it->offset);
                    break;
                case field_type_t::type_uint8:
                    js[it->name] = (unsigned int)*(uint8_t *)(self + it->offset);
                    break;
                case field_type_t::type_int16:
                    js[it->name] = *(int16_t *)(self + it->offset);
                    break;
                case field_type_t::type_uint16:
                    js[it->name] = *(uint16_t *)(self + it->offset);
                    break;
                case field_type_t::type_int32:
                    js[it->name] = *(int32_t *)(self + it->offset);
                    break;
                case field_type_t::type_uint32:
                    js[it->name] = *(uint32_t *)(self + it->offset);
                    break;
                case field_type_t::type_int64:
                    js[it->name] = *(int64_t *)(self + it->offset);
                    break;
                case field_type_t::type_uint64:
                    js[it->name] = *(uint64_t *)(self + it->offset);
                    break;
                case field_type_t::type_float32:
                    js[it->name] = *(float *)(self + it->offset);
                    break;
                case field_type_t::type_float64:
                    js[it->name] = *(double *)(self + it->offset);
                    break;
                case field_type_t::type_fix_str:
                    js[it->name] = (char *)(self + it->offset);
                    break;
                case field_type_t::type_int_array:
                {
                    std::vector<int> vec;

                    for (int i = 0; i < it->array_size; i++)
                    {
                        if (i < it->array_size - 1)
                        {
                            vec .push_back( *((int *)(self + it->offset + i * sizeof(int))));
                        }
                        else
                        {
                            vec .push_back( *(int *)(self + it->offset + i * sizeof(int)));
                        }
                    }
                    js[it->name] = vec;
                }
                break;
                case field_type_t::type_float64_array:
                {
                    std::vector<double> vec;
                    for (int i = 0; i < it->array_size; i++)
                    {
                        if (i < it->array_size - 1)
                        {
                           vec .push_back( *((double *)(self + it->offset + i * sizeof(double))));
                        }
                        else
                        {
                            vec .push_back( *(double *)(self + it->offset + i * sizeof(double)));
                        }
                    }
                    js[it->name] = vec;
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
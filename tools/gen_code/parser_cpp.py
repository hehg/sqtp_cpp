import CppHeaderParser as parser
import sys
import os
file_path = sys.argv[1]
cpp = parser.CppHeader(file_path)

quot = cpp.classes['sq_quot']
#print(quot)
print(quot['name'])
class_name = quot['name']
for a in quot['properties']['public']:
     f_type = a['type']
     f_name = a['name']
     func_name = ""
     #print(a)
     #print(f_name,f_type)
     if 'array_size' in a.keys():
          f_size=a['array_size']
          if f_type in ['price_t', 'double']:
               func_name = "ADD_FD_FLOAT64_ARRAY"
          elif f_type in ['int', 'qty_t']:
               func_name = "ADD_FD_INT_ARRAY"
          else:
               print("not support "+f_name+" "+f_type)
          s=f"{func_name}({class_name},{f_name},{f_size});"
          print(s)
     else:
          if f_type in ['price_t', 'double']:
               func_name = "ADD_FD_FLOAT64"
          elif f_type in ['float']:
               func_name = "ADD_FD_FLOAT32"
          elif f_type in ['trade_date_t', 'trade_time_t', 'market_id_t', 'variety_t',
     'contract_t', 'user_id_t', 'offer_id_t', 'err_msg_t', 'sys_order_id_t']:
               func_name = 'ADD_FD_STR'
          elif f_type in ['uint8_t', 'trade_type_t', 'bs_flag_t', 'offset_t', 'order_status_t', 'product_type_t']:
               func_name = "ADD_FD_UINT8"
          elif f_type in ['int8_t']:
               func_name = "ADD_FD_INT8"
          elif f_type in ['short']:
               func_name = "ADD_FD_INT16"
          elif f_type in['uint16_t','tid_t']:
               func_name="ADD_FD_UINT16"
          elif f_type in['int','int32_t','qty_t']:
               func_name="ADD_FD_INT32"
          elif f_type in['uint32_t','order_type_t']:
               func_name="ADD_FD_UINT32"
          elif f_type in['int64_t','order_id_t']:
               func_name="ADD_FD_INT64"
          elif f_type in['uint64_t','match_id_t']:
               func_name="ADD_FD_UINT64"
          elif f_type in['bool']:
               func_name="ADD_FD_BOOL"
          else:
               print("not support "+f_name+" "+f_type)
               continue
          s=f"{func_name}({class_name},{f_name});"
          print(s)
# for c in cpp.classes.keys():
#     my=cpp.classes[c]
#     print(my.name)
#     print(my)
    # for a in my['properties']['public']:
    #     print(a['type'],a['name'])
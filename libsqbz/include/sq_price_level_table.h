#pragma once
#include<map>
#include "sq_struct_def.h"
using namespace std;
namespace sq {

    struct order_price_cmp {
        bool operator()(sq_order_record*l, sq_order_record*r)
        {
            return true;
        }
    };
    
    /** 用来存储某一个合约的，订单价格表，根据买卖方向，分类
    并根据订单价格排序，
    买：降序
    卖：升序
    */
  
    class price_level_table
    {
        typedef std::map<int64_t, sq_order_record*>   order_map;
        typedef std::map<price_t, order_map, std::less<price_t>>  price_map_sell;
        typedef std::map<price_t, order_map,std::greater<price_t>>  price_map_buy;
    public:
        enum {
            op_equal=0,
            op_less_eq,
            op_less,
            op_great,
            op_great_eq, //大于等于
        };
        price_level_table() = default;
        ~price_level_table(){}

        bool insert(int id,sq_order_record*order);
        sq_order_record* remove(int bs, price_t price, int64_t id);
        sq_order_record* find(int bs, price_t price, int64_t id);
        void delete_all();

        /** 按价格，方向查找符合条件的订单
        @bs:订单方向
        @price:比较价格
        @op: 操作
        */
        int search(int bs, price_t price, int op,std::vector<sq_order_record*>&out);
        price_map_buy m_buy_list;
        price_map_sell m_sell_list;

        void dump(std::ostream&stream);
    };
   
}
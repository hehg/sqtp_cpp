#include "sq_price_level_table.h"

namespace sq {

    bool price_level_table::insert(int id,sq_order_record*order)
    {
        //buy
        if (order->direction == f_buy) {
            auto it_price= m_buy_list.find(order->price);
            if (it_price == m_buy_list.end()) {
                m_buy_list[order->price] = order_map();
                it_price = m_buy_list.find(order->price);
            }
           
            it_price->second.insert(std::make_pair(id,order));
        }
        else if(order->direction==f_sell){
            auto it_price = m_sell_list.find(order->price);
            if (it_price == m_sell_list.end()) {
                m_sell_list[order->price] = order_map();
                it_price = m_sell_list.find(order->price);
            }
            it_price->second.insert(std::make_pair(id, order));
        }

        return true;
       
    }
    sq_order_record* price_level_table::remove( int bs, price_t price, int64_t id)
    {
        sq_order_record*item = nullptr;
        if (bs == f_buy) {
            auto it_price = m_buy_list.find(price);
            if (it_price != m_buy_list.end()) {
                auto it_item = it_price->second.find(id);
                if (it_item != it_price->second.end()) {
                    item = it_item->second;
                    it_price->second.erase(id);
                 }
            }
            //价位空了
            if (item != nullptr && it_price->second.empty()) {
                m_buy_list.erase(it_price);
            }
        }
        else {
            auto it_price = m_sell_list.find(price);
            if (it_price != m_sell_list.end()) {
                auto it_item = it_price->second.find(id);
                if (it_item != it_price->second.end()) {
                    item = it_item->second;
                    it_price->second.erase(id);
                }
            }
            //价位空了
            if (item != nullptr && it_price->second.empty()) {
                m_sell_list.erase(it_price);
            }

        }
        return item;
    }
    sq_order_record* price_level_table::find(int bs,price_t price, int64_t id)
    {
        sq_order_record*item = nullptr;
        if (bs == f_buy) {
            auto it_price = m_buy_list.find(price);
            if (it_price != m_buy_list.end()) {
                auto it_item = it_price->second.find(id);
                if (it_item != it_price->second.end()) {
                    item = it_item->second;
                    it_price->second.erase(id);
                }
            }
           
        }
        else {
            auto it_price = m_sell_list.find(price);
            if (it_price != m_sell_list.end()) {
                auto it_item = it_price->second.find(id);
                if (it_item != it_price->second.end()) {
                    item = it_item->second;
                    it_price->second.erase(id);
                }
            }
            

        }
        return item;
    }
    int price_level_table::search(int bs, price_t price, int op, std::vector<sq_order_record*>&out)
    {
        if (bs == f_buy) {
            auto it = m_buy_list.begin();
            for (; it != m_buy_list.end(); ++it) {

                if (op == op_equal &&fabs(it->first - price) < 0.0000001)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_less_eq && it->first<=price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_less && it->first < price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_great_eq && it->first >= price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_great && it->first > price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
            }
        }
        else {
            auto it = m_sell_list.begin();
            for (; it != m_sell_list.end(); ++it) {

                if (op == op_equal &&fabs(it->first - price) < 0.0000001)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_less_eq && it->first <= price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_less && it->first < price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_great_eq && it->first >= price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
                else if (op == op_great && it->first > price)
                {
                    for (auto it2 : it->second) {
                        out.push_back(it2.second);
                    }
                }
            }
        }

        return out.size();
    }
    void price_level_table::delete_all()
    {
        {
            auto it_p = m_buy_list.begin();
            for (; it_p != m_buy_list.end(); ++it_p) {

                order_map::iterator it_o = it_p->second.begin();
                for (; it_o != it_p->second.end(); ++it_o) {
                    delete it_o->second;

                }

            }
            m_buy_list.clear();
        }
        {
            auto it_p = m_sell_list.begin();
            for (; it_p != m_sell_list.end(); ++it_p) {

                order_map::iterator it_o = it_p->second.begin();
                for (; it_o != it_p->second.end(); ++it_o) {
                    delete it_o->second;

                }

            }
            m_sell_list.clear();
        }
        
    }
    void price_level_table::dump(std::ostream&stream)
    {
         stream << "buy:" << endl;
      
            auto it_p = m_buy_list.begin();
            for (; it_p != m_buy_list.end(); ++it_p) {
                stream << "price:[" << it_p->first << "]";
                auto it_o = it_p->second.begin();
                for (; it_o != it_p->second.end(); ++it_o) {
                    stream << it_o->first << ",";
                }
                stream << endl;
            }
 
        stream << "sell:" << endl;
       
            auto it_p_s = m_sell_list.begin();
            for (; it_p_s != m_sell_list.end(); ++it_p_s) {
                stream << "price:[" << it_p_s->first << "]";
                auto it_o_s = it_p_s->second.begin();
                for (; it_o_s != it_p_s->second.end(); ++it_o_s) {
                    stream << it_o_s->first << ",";
                }
                stream << endl;
            }
           
        
        
    }
 
}
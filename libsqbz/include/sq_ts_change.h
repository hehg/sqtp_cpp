#pragma once
#include <string>
#include <map>
#include <list>
#include <functional>
using namespace std;
namespace sq
{
    //交易状态 切换处理
    using namespace std;
    using trade_status_change_fun_t=std::function<void(const char* name,int type)>;
    
    struct state_change_rec
    {
        uint64_t start;
        uint64_t stop;
        std::string name;
        int state=0;    //0 -init  1--enter  2--leave
    };
    /**状态切换*/
    class sq_ts_change
    {
    public:
        sq_ts_change(){
         
        };
        void ret_changed_callback(trade_status_change_fun_t fun){
            m_fun=fun;
        }
        void reg_status(string start, string stop, string name);
        void goto_time(uint64_t cur_time);
        std::list<state_change_rec> m_sct;

        trade_status_change_fun_t m_fun=nullptr;
    };
} // namespace sq

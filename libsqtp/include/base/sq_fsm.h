#pragma once
#include <unordered_map>
#include <deque>
#include <vector>
#include <functional>
namespace sq
{

    /**
     * @brief
    */
    //template <class T>
    using state_fun_t = std::function<void(int type, char* msg, int length)>;
    class sq_fsm
    {
        class  sq_fsm_state {
        public:
            sq_fsm_state() = default;
 
            sq_fsm_state(int id,state_fun_t fun) :m_fun(fun),m_id(id) {}

            void add(int ev, int s) {
                m_next_states[ev] = s;
            }
            int get_next(int id) {
                return m_next_states[id];
            }
            state_fun_t m_fun;
            int m_id;
          
            std::unordered_map<int, int> m_next_states;
        };
    public:
        void set_cur_state(int cur) {
            m_cur_id = cur;
        }
        int  reg_state(state_fun_t cur) {
            int id = m_count;
            m_states[id] = sq_fsm_state(id,cur);
            m_count++;
            return id;
        }
        
        void add_trans_table(int cur, int event_id, int next)
        {
            m_states[cur].add(event_id, next);
        }
        void emit_event(int id)
        {
            m_cur_id = m_states[m_cur_id].get_next(id);
            m_cur_state = m_states[m_cur_id].m_fun;
        }
        void exec(int type, char* msg, int length) {
            m_cur_state(type, msg, length);
        }
       
        state_fun_t m_cur_state = nullptr;
        int m_count = 0;
        int m_cur_id=0;
        std::unordered_map<int, sq_fsm_state> m_states;
    };
}
#include "sq_ts_change.h"
#include "sq_convert.h"
namespace sq
{
    void sq_ts_change::reg_status(string start, string stop, string name)
    {

        state_change_rec sct;
		sct.start = datetime_to_int(start.c_str());
		sct.stop = datetime_to_int(stop.c_str());
		sct.name = name;
        sct.state=0;
		m_sct.push_back(sct);
    }
    void sq_ts_change::goto_time(uint64_t cur_time)
    {
        for (auto & a : m_sct)
        {
            if (cur_time >= a.start&&cur_time<a.stop  && a.state==0){
                a.state=1;//enter
                if(m_fun)
                    m_fun(a.name.c_str(),a.state);
            }
            else if(cur_time >= a.stop  && (a.state==1||a.state==0)){
                a.state=2;//leave
                if(m_fun)
                    m_fun(a.name.c_str(),a.state);
            }
        }
    }
} // namespace sq


#include "replay_quot.h"
#include <random>
#include <time.h>
#include <math.h>
#include "log/sq_logger.h"
#include "time/date_time.h"
#include "sq_struct_def.h"

using namespace sq;
namespace sq_plug
{
    replay_quot::replay_quot()
    {
        m_name="md_replay";
    }
    replay_quot::~replay_quot()
    {

    }

    int replay_quot::open()
    {
        plug_base::open();

        run_flag_ = true;
        m_engine = new std::thread(&replay_quot::run, this);
        log_info("open plug replay_quot\n");
        SQ_LOG_FLUSH();
        return 0;
    }
    int replay_quot::close()
    {
        run_flag_ = false;
        if (m_engine && m_engine->joinable())
        {
            m_engine->join();
        }
        if (m_engine)
        {
            delete m_engine;
            m_engine = nullptr;
        }
        return 0;
    }

    void replay_quot::run()
    {
        srand(time(0));
        int pkg_no=0;
        while (run_flag_)
        {
            date_time now=date_time::now();
            os_sleep_ms(1000);
            sq_quot quot;
            quot.tid = tid_market_data;
            quot.pkg_no = pkg_no++;
            
            quot.date=now.format("YYYYMMDD");
            quot.time=now.format("HH:mm:SS.sss");
            quot.market = market_dce;
            quot.variety = "a";
            sprintf(quot.contract.data(), "a23%02d",rand()%10);
            quot.price = rand();
            quot.match_qty = pkg_no+100;
            quot.turnover = pkg_no+1000;
            quot.interest = 3;
            for (int i = 0; i < 5; i++)
            {
                quot.mbl_buy_price[i] = i;
                quot.mbl_buy_qty[i] = i;
                quot.mbl_sell_price[i] = i;
                quot.mbl_sell_qty[i] = i;
            }
            quot.open = 0;
            quot.close = 0;
            quot.high = 0;
            quot.low = 0;
            quot.limit_down = 0;
            quot.limit_up = 0;
            quot.mstime = 0;
            quot.recv_time = 0;
            quot.pre_close = 0;
            quot.multi = 1;
            if (m_call_back)
            {
                m_call_back(tid_market_data, (char *)&quot, sizeof(quot), m_call_back_param, m_id);
            }
           
            // std::cout << "run\n";
        }
    }
}
#include "bz_stock_fe.h"
#include "log/sq_logger.h"
#include "error/sq_err.h"
#include "text/csv_file.h"
#include "sq_struct_def.h"
#include "time/date_time.h"
#include "sq_const_define.h"

using namespace sq;
namespace sq_plug
{
        bz_stock_fe::bz_stock_fe()
        {
        }
        bz_stock_fe::~bz_stock_fe()
        {
        }
        int bz_stock_fe::open()
        {
                m_fes[0] = &m_stock_quote_price;
                m_fes[1] = &m_stock_moments;
                m_fes[2] = &m_stock_big_order;
                // open sockt list
                std::string profile_path = get_cfg_string("stock_list");
                csv_file_reader reader;
                bool ret = reader.open(profile_path.c_str(), true);
                if (ret == false)
                {
                        cout << "open file error" << endl;
                        assert(false);
                }
                SQ_LOGV(log_info, "open_profile %s\n", profile_path.c_str());
                while (reader.read_row())
                {
                        string c = reader["ContractID"].as_string();
                        double s = reader["FreeFloatShares"].as_double();
                        double w = reader["Weight"].as_double();
                        int idx = atoi(c.c_str());
                        m_support_stocks[c] = idx;
                        for (int i = 0; i < m_fes_size; i++)
                        {
                                m_fes[i]->add_stock(idx, s, w);
                        }
                }
                m_dest_tid=get_cfg_int("out_put_tid");
                assert(m_dest_tid);
                m_result.tid = m_dest_tid;
                return ok;
        }
        int bz_stock_fe::close()
        {
                return ok;
        }
        void bz_stock_fe::run()
        {
        }
        void bz_stock_fe::handle_time_for_reocver()
        {
                date_time dt = date_time::make_from_timestamp(m_cur_timestamp);
                uint64_t cur_time = 1000 * (dt.m_hour * 3600 + dt.m_minute * 60 + dt.m_second) + dt.m_msec;
                if ((cur_time >= timestamp090000 && cur_time <= timestamp113000) ||
                    (cur_time >= timestamp130000 && cur_time <= timestamp150000))
                {
                        m_is_open_state = true;
                }
                else
                {
                        m_is_open_state = false;
                }

                char str_time[64];
				sprintf(str_time, "%02d%02d%02d%03d", dt.m_hour, dt.m_minute, dt.m_second, dt.m_msec);
                m_int_time=atoi(str_time);
        }
        int bz_stock_fe::put(uint16_t tid, char *data, uint16_t size)
        {
                uint64_t timestamp=0;

                std::string contract;
                if (tid == tid_market_data)
                {
                        sq_quot *quot = (sq_quot *)data;
                        timestamp = quot->recv_time;
                        contract = quot->contract.data();

                }
                else if (tid == tid_stock_match_quot)
                {
                        stock_match_quot *quot = (stock_match_quot *)data;
                        timestamp=quot->recv_time;
                        contract=quot->contract.data();
                        
                }
                else{
                        return ok;
                }
                auto it = m_support_stocks.find(contract);
                if (it == m_support_stocks.end())
                {
                        return ok;
                }
                //100ms 更新一次
                if (timestamp > m_cur_timestamp && timestamp-m_cur_timestamp>1000*100)
                {
                        m_cur_timestamp = timestamp;
                        handle_time_for_reocver();
                }

                if (m_is_open_state)
                {
                        for (int i = 0; i < m_fes_size; i++)
                        {
                                m_fes[i]->put(tid, data, size);
                        }

                        uint64_t diff = ((m_cur_timestamp / m_timer_interval) - (m_last_time / m_timer_interval));
                        if (diff > 0)
                        {
                                m_last_time = m_cur_timestamp;

                                

                                for (int i = 0; i < m_fes_size; i++)
                                {
                                        m_fes[i]->calc_result(&m_result);
                                }
                               
                                m_result.time = m_int_time;
                                m_result.localTimeStamp = m_cur_timestamp;
                                m_result.number++;

                                if(m_call_back){
                                        m_call_back(m_result.tid,(char*)&m_result,sizeof(m_result),m_call_back_param,m_id);
                                }
                        }
                }


                return ok;
        }
}
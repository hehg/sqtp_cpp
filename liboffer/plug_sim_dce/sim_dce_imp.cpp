#include "sim_dce_imp.h"
#include "log/sq_logger.h"
namespace sq_plug
{
    sim_dce_imp::sim_dce_imp()
    {
        m_name = "md_replay";
    }
    sim_dce_imp::~sim_dce_imp()
    {
    }
    int sim_dce_imp::open()
    {
        plug_base::open();

        run_flag_ = true;
        m_engine = new std::thread(&sim_dce_imp::run, this);

        //加载基础数据
        log_info("open plug sim_dce_imp\n");
        SQ_LOG_FLUSH();
        return ok;
    }
    int sim_dce_imp::close()
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
    void sim_dce_imp::run()
    {
         while (run_flag_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    void sim_dce_imp::init_data()
    {
        std::string db_dir=get_cfg_string("db_dir");
        //加载合约表
        std::string path=db_dir+"/contract.txt";
        m_contract_table.load_from_text(path.c_str());
        m_contract_table.dump(std::cout);
    }
    int sim_dce_imp::put(uint16_t tid, char* data, uint16_t size)
    {
        return ok;
    }
}
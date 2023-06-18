#include "store_quot.h"
#include "log/sq_logger.h"
#include "time/date_time.h"
#include "sq_tid_define.h"
#include "os/sq_path.h"
using namespace sq;
namespace sq_plug
{
	store_quot::store_quot(/* args */)
	{
	}

	store_quot::~store_quot()
	{
	}
	int store_quot::open()
	{
		plug_base::open();

		//config
		auto markets = get_cfg_arrary("markets");
		for (auto it : markets) {
			m_markets.insert(it);
		}
		auto symbols = get_cfg_arrary("symbols");
		for (auto it : symbols) {
			m_symbols.insert(it);
		}
		auto tids = get_cfg_arrary_int("tids");
		for (auto it : tids) {
			m_tids.insert(it);
		}

		string name = get_cfg_string("name", "so");
	    string out_dir = get_cfg_string("out_dir", "./");
		bool have_head = get_cfg_bool("have_head");

		string file_name;
		sq::make_path(file_name, out_dir.c_str(), name.c_str());
		if (sq_path::exist(file_name.c_str())) {
			m_writer.open(file_name.c_str(), "a+", false);
		}
		else {
			m_writer.open(file_name.c_str(), "a+", have_head);
		}
		//start thread
		run_flag_ = true;
		m_engine = new std::thread(&store_quot::run, this);
		log_info("start plug store_quot\n");

		return ok;
	}
	int store_quot::close()
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
		return ok;
	}
	void store_quot::run()
	{
		while (run_flag_)
		{
			os_sleep_ms(1000);
			
		}
	}
	int store_quot::put(uint16_t tid, char* data, uint16_t size)
	{
		if (!m_tids.empty()) {
			if (m_tids.find(tid) == m_tids.end()) {
				return ok;
			}
		}
		if (tid == tid_market_data) {

			m_writer.write(tid, data, size);
		}
		return ok;
	}
}
#include "plug_base.h"
#include <thread>
#include <set>
#include <string>
#include "sq_quot_file.h"
/**
 * 行情存储插件
*/
namespace sq_plug
{
    class store_quot: public plug_base
    {
    public:
        store_quot(/* args */);
        ~store_quot();

        int open();
        int close();
		int put(uint16_t tid, char* data, uint16_t size);
		void run();

		std::set<string>	m_markets;	//support market name
		std::set<string>	m_symbols;  //support contract or stocks
		std::set<int>		m_tids;

		
		bool run_flag_ = false;
		std::thread *m_engine = nullptr;
		sq_quot_file_writer m_writer;

    };
}
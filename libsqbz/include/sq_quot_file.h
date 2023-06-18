#pragma once
#include <stdio.h>
#include <vector>
#include <fstream>
#include "sq_define.h"
#include "sq_struct_def.h"

using namespace std;
namespace sq{
	/**
	各个列的id
	*/
	enum {
		Id_date = 0,
		Id_time, 
        Id_contract, 
        Id_price, 
        Id_match_qty,
		Id_interest,
		Id_BP1, Id_BP2, Id_BP3, Id_BP4, Id_BP5,
		Id_BV1, Id_BV2, Id_BV3, Id_BV4, Id_BV5,

		Id_SP1, Id_SP2, Id_SP3, Id_SP4, Id_SP5,
		Id_SV1, Id_SV2, Id_SV3, Id_SV4, Id_SV5,
		Id_Recvtime,
		Id_limit_up, 
        Id_limit_down, 
        Id_turnover,
		Id_market, 
        Id_open,
        Id_high,
        Id_low, 
        Id_timems,
		Id_variety,
        Id_tid,
        Id_type,
        Id_pkg_no,
        Id_close,
        Id_pre_close,
		Id_multi,
        Id_unknow,
	};
   std::string format_price(double price);
    //列信息
    struct column_info {
        short col;  //第几列
        string col_name;//列的名称
        short id;//列的标识
       // char  data_type;//数据类型
    };
    class sq_quot_file
    {
        public:
        sq_quot_file();
        ~sq_quot_file();
        int close();
        //自定义头
        void set_header(const char*hdr);
        protected:
        std::fstream m_file;
        std::vector<column_info> m_cols;//表头信息
        map<string, int> m_dict_name;
    };
    /**
     * 行情数据写入
     */
    class sq_quot_file_writer : public sq_quot_file
    {
    public:
        sq_quot_file_writer();
        int open(const char *path, const char *mode = "w+", bool have_head = true);
        int write(int tid, char *data, int size);
    };

    /**
	行情数据读取
	*/
	class sq_quot_file_reader:public sq_quot_file
	{
	public:
		sq_quot_file_reader();
		~sq_quot_file_reader();

		bool open(const char*path, bool hasHead = true);
		bool read_line(sq_quot &quot);
		vector<string>   m_tmp_out;

	};

}

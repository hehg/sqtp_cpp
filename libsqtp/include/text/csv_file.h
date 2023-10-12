#pragma once
#include <vector>
#include <string>
#include <map>
#include <limits>
using namespace std;

namespace sq
{
	class csv_file
	{
	public:
		csv_file();
		~csv_file();
        typedef vector<string> line_list_t;
        typedef vector<string> field_list_t;
		bool open(const char*path, const char*mode = "r");
        void close();
		//读取一条记录
		bool read(field_list_t&records);
		void write(field_list_t&line);

        
        int32_t read_all_line(line_list_t &lists);
	private:
		int read_line(char *buf, int buflen);
		void write_line(const char* buf);
	private:
		FILE* m_file;
		char m_read_buf[10240];
		char m_seperate;
	};

	/** csv 文件扩展
	@code
	csv_file_ex csv;
	csv.open("./head.csv");
	csv.read_header();

	while (csv.read_row())
	{
	std::cout << csv["h1"] << "|" << csv["h2"] << "|" << csv["h3"] << "|" << endl;
	}
	@endcode
	*/

	class csv_file_reader :public csv_file
	{
		
	public:
		struct csv_field
		{
			string val;
			string as_string() { return val; }
			int    as_int() { return atoi(val.c_str()); }
			double    as_double() { return atof(val.c_str()); }
			double    as_price(){
				if(val!="null") return atof(val.c_str());
				else return (std::numeric_limits<double>::max)();
			}
			int64_t    as_int64() { return atoll(val.c_str()); }
			bool       as_bool() { return  atoi(val.c_str()) > 0; }
		};
        csv_file_reader(){}
		~csv_file_reader(){}

        bool open(string path, bool has_head=true);
		
		void set_header(vector<string> heads);
        //读取一行数据到field中
		bool read_row();
		int  get_col_count() { return m_tmp_records.size(); }
		vector<string>&get_head_list() { return m_head_list; }
		csv_field operator [](int field_idx);
		csv_field operator [](const char* field_name);

        csv_file_reader& operator >> (int &val);
        csv_file_reader& operator >> (double &val);
        csv_file_reader& operator >> (string &val);
        csv_file_reader& operator >> (char* val);
    protected:
        void read_header();
	private:
		std::map<string, int> m_name_index;
		vector<string> m_head_list;
		int m_cur_idx;
		vector<string> m_tmp_records;
	};
}

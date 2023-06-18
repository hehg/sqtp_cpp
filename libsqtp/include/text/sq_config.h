#pragma once
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;
namespace sq
{
    /**文本格式的配置文件
    格式如 :
    key=val
    支持数组形式:
    key=val1,val2
    */
	class sq_config
	{
	public:
		sq_config();
		~sq_config();

		bool open_file(const char *path);
		void load_string(const char*val);
		void parse(int argc, char*argv[]);

		string 	get_string(const char*key,string default_val="");
		int 	get_string_array(const char*key, vector<string> &result);
		int     get_int_array(const char*key,vector<int>&result);
		int    	get_int(const char*key, int default_val =0);
		double  get_double(const char*key, double default_val = 0);
		bool   	get_bool(const char*key, bool default_val = true);


		std::unordered_map<string, string> m_key_val;
	};

    

}
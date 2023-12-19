#pragma once
#include "sq_define.h"

namespace sq
{
	/**
	 * @brief 对于如下格式的数据进行自动解析
	 * key1=val1,key2=val2,.....
	*/
	class kv_parser
	{
	public:
		kv_parser() {}

		void parser(const char*str);
		void parser(string&str);
		
		string & operator [](const char*key);
		string 	get_as_string(const char*key);
		bool 	get_as_bool(const char*key);
		int 	get_as_int(const char*key);
		int64_t 	get_as_int64(const char*key);
		double 	get_as_double(const char*key);

		string dump();
		unordered_map<string, string> m_kv;
	};
}
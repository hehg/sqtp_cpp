#pragma once
#include "sq_define.h"

namespace sq
{
	class kv_parser
	{
	public:
		kv_parser() {}

		void parser(const char*str);
		void parser(string&str);
		
		string & operator [](const char*key);
		string get_as_string(const char*key);
		bool get_as_bool(const char*key);
		int get_as_int(const char*key);
		double get_as_double(const char*key);

		string dump();
		unordered_map<string, string> m_kv;
	};
}
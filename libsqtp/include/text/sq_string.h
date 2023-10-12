/**
Copyright (c) 2003-2015 
@brief 字符串相关操作


@author 
*/
#pragma once
#include "sq_define.h"
#include <string.h>
namespace sq{

	string sq_string_trim(string& s);

	int   sq_split(const char*str, vector<string>& out, const char seperate = ',');
    //int  sq_split(const char* string_org, unsigned char*end, vector<string>& out,
    //    const char seperate);
    bool compare_no_case(const char*s1, const char*s2);

	string& replace_all(string&str, string old_val, string new_val);
	
	bool ends_with(string src, string end);
	bool starts_with(string src, string begin);

	void to_lower(string &src);
	void to_upper(string &src);
	//类型转换函数
	template <typename T>
	T sq_from_string(const std::string &str)
	{
		T result;
		std::istringstream iss(str);

		iss >> result;

		return result;
	}
	int str_to_int(const string &s);
    string int_to_str(int val);

	double str_to_double(const string &s);
    string double_to_str(double val);
   
	//按分隔符连接字符串
	string join_list(vector<string> &list,const string sep);

	void safe_strncpy(char*des, int des_size, const char*src);

	//按模板生成字符串
    string template_string(const char*src, std::map<string, string>&dict);
   
    int sq_split_csv(const string &src, std::vector<string> &out);

	int sq_split_by_regex(const string &src, const char* reg_express, std::vector<string> &out);
}

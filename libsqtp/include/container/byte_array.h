/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once

#include "sq_define.h"
#include "sq_macro.h"

namespace sq
{
	template<int N>
	struct byte_array 
	{
		byte_array(){}

		explicit byte_array(byte_array &val)
		{
			safe_strcpy(data(), N,val.data());
		}
		byte_array(std::string &val)
		{
			safe_strcpy(data(), N,val.c_str());
		}
		byte_array(const std::string &val)
		{
			safe_strcpy(data(),N, val.c_str());
		}
		byte_array(const char*val)
		{
			safe_strcpy(data(), N, val);
		}
        byte_array& operator =(std::string &val)
		{
			safe_strcpy(data(), N, val.c_str());
            return *this;
		}
        byte_array& operator =(const std::string &val)
		{
			safe_strcpy(data(), N, val.c_str());
            return *this;
		}
        byte_array& operator =(const char*val)
		{
			safe_strcpy(data(), N, val);
            return *this;
		}
		size_t hash() {
			return std::hash<string>()(data());
		}
		bool operator ==(byte_array&val)
		{
			return strcmp(data(), val.data()) == 0;
		}
		bool operator !=(byte_array&val)
		{
			return strcmp(data(), val.data()) != 0;
		}
		bool operator ==(const char*val)
		{
			return strcmp(data(), val) == 0;
		}
		bool operator !=(const char*val)
		{
			return strcmp(data(), val) != 0;
		}
		bool operator <(const byte_array&left)const {
			int cmp = strcmp(data(), left.data());
			return cmp == -1;
		}
        bool empty() {
            return strlen(data()) == 0;
        }
		string to_string()
		{
			string s(data());
			return s;
		}
		char* data() { return m_data; }
		const char* operator ()()
		{
			return data();
		}
		int size() { return N; }

		char m_data[N];
	};
}

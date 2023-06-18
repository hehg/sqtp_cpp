#include "text/sq_string.h"
#include <string.h>
#include <regex>
namespace sq{

    bool compare_no_case(const char*s1, const char*s2)
    {
#ifdef WINDOWS
        return stricmp(s1, s2) == 0;
#elif Cygwin
			//return stricmp(s1, s2) == 0;
			//strupr()
			return false;
#else
        return strcasecmp(s1, s2) == 0;
#endif
    }
	int  sq_split(const char* string_org,vector<string>& out, const char seperate)
	{
		if (strlen(string_org)==0){
			return 0;
		}
		char*p = (char*)string_org;
		char*pre = p;
		while (*p != 0) {
			if (*p == seperate) {
				int len = p - pre;
				if (len > 0) {
					string s(pre, len);
					out.emplace_back(s);
					pre = ++p;
				}
				else {
					out.push_back("");
					pre = ++p;
				}

			}
			else {
				p++;
			}

		}
		if (pre != p) {
			int len = p - pre;
			if (len > 0) {
				string s(pre, len);
				out.emplace_back(s);
			}
		}
		else if (pre == p&&p != 0 /*&& *p == seperate*/) {
			out.push_back("");
		}
		return out.size();
	}

	string sq_string_trim(string& s)
	{
		if (s.empty())
			return "";
		if (s.length() == 1) {
			string ss = s;
			if (s[0] == ' ' || s[0] == '\t' || s[0] == '\n'||s[0]=='\r')
			{
				ss = "";
			}
			return std::move(ss);
		}
		int i = 0;
		while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n'|| s[i] == '\r')//开头处为空格或者Tab，则跳过
		{
			i++;
		}
		s = s.substr(i);
		i = s.size() - 1;
		while (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')////结尾处为空格或者Tab，则跳过
		{
			i--;
		}
		if(i==0){
           return "";
		}
		else{
			string ss = s.substr(0, i+1);
			return std::move(ss);
		}
		
	}
	string& replace_all(string&str, string old_val, string new_val)
	{
		while (true)   {
			string::size_type   pos(0);
			if ((pos = str.find(old_val)) != string::npos)
				str.replace(pos, old_val.length(), new_val);
			else
				break;
		}
		return   str;
	}
	bool ends_with(string src, string end)
	{
		string sub = src.substr(src.size()-end.size(),end.size());
		return sub == end;
	}
	bool starts_with(string src, string begin)
	{
		string sub = src.substr(0, begin.size());
		return sub == begin;
	}
	void to_lower(string &src)
	{
		auto it = src.begin();
		for (; it != src.end(); ++it) {
			*it = tolower(*it);
		}
	}
	void to_upper(string &src)
	{
		auto it = src.begin();
		for (; it != src.end(); ++it) {
			*it = toupper(*it);
		}
	}
	int str_to_int(const string &s)
	{
		return atoi(s.c_str());
	}
	double str_to_double(const string &s)
	{
		return strtod(s.c_str(),nullptr);
	}
    string double_to_str(double val)
    {
        char tmp[64] = { 0 };
        sprintf(tmp, "%f", val);
        string ss(tmp);
        return std::move(ss);
    }
    string int_to_str(int val)
    {
        char tmp[64] = { 0 };
        sprintf(tmp, "%d", val);
        string ss(tmp);
        return std::move(ss);
    }
	string join_list(vector<string> &list, const string sep)
	{
		string ret;
		for (size_t i = 0; i < list.size(); i++)
		{
			ret = ret + list[i];
			if (i < list.size() - 1)
			{
				ret = ret + sep;
			}
		}
		return ret;
	}
	void safe_strncpy(char*des, int des_size, const char*src)
	{
		int src_size = strlen(src);
		int real_size = des_size > src_size ? src_size : des_size - 1;
		strncpy(des, src, real_size);
		des[real_size] = '\0';
	}

    string template_string(const char*src, std::map<string, string>&dict)
    {
        char*p = (char*)src;
        std::stringstream out;
        while (*p != '\0')
        {
            if (*p == '$'&& *(p + 1) == '{')
            {
                p += 2;
                char*begin = p;
                while (*p != '}') ++p;
                char*end = p;
                string key(begin, end);
                auto it = dict.find(key);
                if (it != dict.end()) {
                    out << it->second;
                }
                else {
                    printf("not match key\n");
                }
                ++p;
            }
            else {
                out << *p;
                ++p;
            }
        }
        return out.str();
	}

	int sq_split_csv(const string &src, std::vector<string> &out)
	{
		std::regex reg("^(.+),(.+)*,(.+)$");
		std::smatch ret;
		bool is_ok = std::regex_match(src, ret, reg);
		if (!is_ok)
		{
			return 0;
		}
		auto it = ret.begin() + 1;
		for (; it != ret.end(); it++)
		{
			out.push_back(*it);
		}
		return out.size();
	}

	int sq_split_by_regex(const string &src, const char* reg_express,
	 std::vector<string> &out)
	{
		std::regex reg(reg_express);
		std::smatch ret;
		bool is_ok = std::regex_match(src, ret, reg);
		if (!is_ok)
		{
			return 0;
		}
		auto it = ret.begin() + 1;
		for (; it != ret.end(); it++)
		{
			out.push_back(*it);
		}
		return out.size();
	}
}
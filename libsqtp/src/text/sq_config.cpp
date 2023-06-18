#include "text/sq_config.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include "text/sq_string.h"
#include "base/sq_util.h"
using namespace std;
namespace sq
{

	sq_config::sq_config()
	{
	}


	sq_config::~sq_config()
	{
	}
	bool sq_config::open_file(const char *path)
	{
		string format=sq_util::check_file_format(path);
		if(format!="ANSI"){
			printf("WARN not support file format [%s]\n",format.c_str());
		}
		char buf[4096];
		m_key_val.clear();
		fstream fin;
		fin.open(path, ios::in);
		if (!fin.is_open())
		{
			return false;
		}

		string cur_key="";
		while (fin.getline(buf, sizeof(buf)))
		{
			if (strlen(buf) == 0)
				continue;

            string tmp(buf);
			string str_tmp=sq_string_trim(tmp);
			if(str_tmp.empty()){
				continue;
			}
			
            
			if (str_tmp[0] == ';' || str_tmp[0] == '#')
				continue;

			int len = str_tmp.length();
			int pos = 0;
			for (; pos < len; pos++)
			{
				if (str_tmp[pos] == '=')
					break;
			}

			if (pos>0 && pos < len)
			{
				string key(str_tmp.begin(), str_tmp.begin() + pos);
				cur_key=key;
				string val(str_tmp.begin() + pos + 1, str_tmp.end());
				key = sq_string_trim(key);
				val= sq_string_trim(val);
				m_key_val[key] = val;

				cout << key<<":"<<val<<endl;
			}
			else{
				m_key_val[cur_key]+=str_tmp;
				cout << cur_key<<"="<<m_key_val[cur_key]<<endl;
			}
		}

		fin.close();

		return true;
	}

	void sq_config::parse(int argc, char*argv[])
	{
		if (argc == 1)
			return;
		for (int i = 1; i < argc; i++)
		{
			char*line = argv[i];

			int len = strlen(line);
			int pos = 0;
			for (; pos < len; pos++)
			{
				if (*(line + pos) == '=')
					break;
			}

			if (pos>0 && pos < len)
			{
				string key(line, line + pos);
				string val(line + pos + 1);

				key = sq_string_trim(key);
				m_key_val[key] = val;

				// cout << key<<":"<<val<<endl;
			}
		}

	}
	


	string sq_config::get_string(const char*key, string default_val)
	{
		if (m_key_val.find(key) != m_key_val.end())
		{
			return m_key_val[key];
		}
		else
		{
			return default_val;
		}
	}
	int sq_config::get_string_array(const char*key, vector<string> &result)
	{
		if (m_key_val.find(key) != m_key_val.end())
		{
			string ss=sq_string_trim(m_key_val[key]);
			sq_split(ss.c_str(), result);
			return result.size();
		}
		else
		{
			return 0;
		}
	}
	int  sq_config::get_int_array(const char*key,vector<int>&result)
	{
		if (m_key_val.find(key) != m_key_val.end())
		{
			string ss=sq_string_trim(m_key_val[key]);
			std::vector<string> _r;
			sq_split(ss.c_str(), _r);
			for(size_t i=0;i<_r.size();i++)
			{
				result.push_back(atoi(_r[i].c_str()));
			}
			return result.size();
		}
		else
		{
			return 0;
		}
	}

	int  sq_config::get_int(const char*key, int default_val)
	{

		if (m_key_val.find(key) != m_key_val.end())
		{
			return atoi(m_key_val[key].c_str());
		}
		else
		{
			return default_val;
		}
	}

	double  sq_config::get_double(const char*key, double default_val)
	{
		if (m_key_val.find(key) != m_key_val.end())
		{
			return atof(m_key_val[key].c_str());
		}
		else
		{
			return default_val;
		}
	}
	bool   sq_config::get_bool(const char*key, bool default_val)
	{
		if (m_key_val.find(key) != m_key_val.end())
		{
			if (m_key_val[key] == "true")
				return true;
			else
				return false;
		}
		else
		{
			return default_val;
		}
	}
}
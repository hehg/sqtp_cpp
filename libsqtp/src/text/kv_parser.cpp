#include "text/kv_parser.h"
#include "text/sq_string.h"
namespace sq
{
	void kv_parser::parser(string&str)
	{
		parser(str.c_str());
	}
	void kv_parser::parser(const char*str)
	{
		m_kv.clear();
		vector<string> out;
		sq_split(str, out);
		for (size_t i = 0; i < out.size(); i++)
		{
			size_t pos = out[i].find_first_of('=');
			if (pos != string::npos)
			{
				string key = out[i].substr(0, pos);
				string val = out[i].substr(pos + 1, out[i].length() - pos-1);

				m_kv[key] = val;
			}
		}
	}
	string& kv_parser::operator [](const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return it->second;
		}
		else
		{
			m_kv.insert(std::make_pair(key, ""));
			return m_kv[key];
		}
		//return get_as_string(key);
	}
	/*template<typename T=int>
	T kv_parser::get_as(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return std::atoi(it->second.c_str());
		}
		return 0;
	}*/
	string kv_parser::get_as_string(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return it->second;
		}
		return "";
	}
	bool kv_parser::get_as_bool(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return it->second=="true";
		}
		return false;
	}
	int kv_parser::get_as_int(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return std::atoi(it->second.c_str());
		}
		return 0;
	}
	int64_t kv_parser::get_as_int64(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return std::atoll(it->second.c_str());
		}
		return 0;
	}
	double kv_parser::get_as_double(const char*key)
	{
		auto it = m_kv.find(key);
		if (it != m_kv.end())
		{
			return std::atof(it->second.c_str());
		}
		return 0;
	}
	string kv_parser::dump()
	{
		stringstream ss;
		for (auto k : m_kv)
		{
			ss << k.first << "=" << k.second << ";";
		}
		return ss.str();
	}
}
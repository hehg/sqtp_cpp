#include "text/csv_file.h"
#include <stdio.h>
#include <string.h>
#include "text/sq_string.h"
#include "log/sq_logger.h"
namespace sq
{
	csv_file::csv_file() :
		m_file(NULL)
	{
		m_seperate = ',';
	}


	csv_file::~csv_file()
	{
		if (m_file)
			fclose(m_file);

	}
	bool csv_file::open(const char*path, const char*mode)
	{
		m_file = fopen(path, mode);
		return m_file != NULL;
	}
    void csv_file::close() 
    {
        if (m_file)
            fclose(m_file);
        m_file = nullptr;
    }
	int csv_file::read_line(char *buf, int buflen)
	{
		int len=0;
		if (m_file)
		{
			char* ret = fgets(buf, buflen, m_file);
			if (ret == NULL)
				return 0;
			len = strlen(buf);
			if(len<buflen){
				buf[len] = '\n';
			}
			else{
				sq_panic("csv_file::read_line() read line too long");
			}
		}
		return len+1;
	}


	//在末尾写入一行数据
	void csv_file::write_line(const char* buf)
	{
		if (m_file)
		{
			fputs(buf, m_file);
			fputs("\n", m_file);
		}
	}

	void csv_file::write(field_list_t&line)
	{
		int count = line.size();
		string strTmp;

		for (int i = 0; i < count; i++)
		{
			strTmp = strTmp + line[i];
			if (i < count - 1)
				strTmp += ",";
		}
		write_line(strTmp.c_str());

	}
	bool csv_file::read(field_list_t &line)
	{
		enum
		{
			STATE_START,
			STATE_QUOTE, // 引号
			STATE_FIELD,
			STATE_QUOTE_QUOTE,
			STATE_QUOTE_COMMA,
			STATE_QUOTE_COMMA_QUOTE,
			STATE_COMMA, // 逗号
			STATE_COMMA_QUOTE,
			STATE_COMMA_COMMA,
			STATE_COMMA_COMMA_QUOTE,
		};
		line.clear();

		int size = read_line(m_read_buf, sizeof(m_read_buf));
		if (size)
		{
			//按 csv 解析这行数据
			char *p = m_read_buf;
			int state = STATE_START;
			string one;
			for (int i = 0; i < size; i++)
			{
				char c = *(m_read_buf + i);
				//行首
				if (state == STATE_START)
				{
					if(c=='\r'||c=='\n'){
						break;
					}
					
					else if (c == ',')
					{
						state = STATE_FIELD;
						line.push_back(one);
						//std::cout << one << "\n";
						one = "";
					}
					else if(c=='"'){
						state = STATE_QUOTE;
					}
					else {
						state = STATE_FIELD;
						one += c;
					}
					
				}
				else if(state==STATE_FIELD)
				{
				    if (c == ',')
					{
						state = STATE_COMMA;
						line.push_back(one);
						//std::cout << one << "\n";
						one = "";
					}
					else if(c=='\r'||c=='\n'){
						line.push_back(one);
						//std::cout <<"endline,"<< one << "\n";
						one = "";
						break;
					}
					else if(c=='"'){
						state = STATE_QUOTE;
					}
					else {
						one += c;
					}
				}
				else if(state==STATE_COMMA)
				{
					if (c == ',')
					{
						state = STATE_COMMA;
						line.push_back(one);
						//std::cout << one << "\n";
						one = "";
					}
					else if(c=='"'){
						state = STATE_QUOTE;
					}
					else if(c=='\r'||c=='\n'){
						line.push_back(one);
						//std::cout<<"endline," << one << "\n";
						one = "";
						break;
					}
					else{
						one += c;
						state = STATE_FIELD;
					}
				}
				else if(state==STATE_QUOTE)
				{
					if (c == '"')
					{
						state = STATE_QUOTE_QUOTE;
						line.push_back(one);
						//std::cout << one << "\n";
						one = "";
					}
					else{
						one += c;
					}
				}
				else if(state==STATE_QUOTE_QUOTE)
				{
					if (c== ','){
						state = STATE_FIELD;
						one = "";
					}
					else if(c=='\r'||c=='\n')
					{
						break;
					}
				}
			}

		}
		else{
			return false;
		}
		return true;
	}
	int32_t csv_file::read_all_line(line_list_t&lists)
    {
        if (m_file)
        {
            char buf[4096];
            while (true) {
                char* ret = fgets(buf, sizeof(buf), m_file);
                if (ret == NULL)
                    break;
                int len = strlen(buf);
                if (len > 0 && buf[len - 1] == '\n')
                    buf[len - 1] = '\0';
                if (len > 1 && buf[len - 2] == '\r')
                    buf[len - 2] = '\0';
                lists.push_back(buf);
            }
        }
        return lists.size();
    }

	//==================csv_file_reader=====================
    bool csv_file_reader::open(string path, bool has_head)
    {
        bool ret= csv_file::open(path.c_str());
        if (has_head) {
            read_header();
        }
        return ret;

    }
	void csv_file_reader::read_header()
	{
		m_name_index.clear();
		read(m_head_list);
		//printf("title=%s");
		for (size_t i = 0; i < m_head_list.size(); i++)
		{
			string name=sq_string_trim(m_head_list[i]);
			//printf("%s,",name.c_str());
			m_name_index[name] = i;
		}
		//printf("\n");
	}
	void csv_file_reader::set_header(vector<string> heads)
	{
		m_name_index.clear();
		m_head_list.swap(heads);
		for (size_t i = 0; i < m_head_list.size(); i++)
		{
			m_name_index[m_head_list[i]] = i;
		}
	}
	bool csv_file_reader::read_row()
	{
		m_cur_idx = 0;
		return read(m_tmp_records);
	}

	csv_file_reader::csv_field csv_file_reader::operator[](int idx)
	{
		csv_field field;
		if (idx >= 0 && idx < (int)m_tmp_records.size())
		{
			field.val = m_tmp_records[idx];
		}
		else if (idx < 0)
		{
			int index = m_tmp_records.size() + idx;
			field.val = m_tmp_records[idx];
		}
		return field;
	}

	csv_file_reader::csv_field csv_file_reader::operator[](const char *field_name)
	{
		csv_field field;
		auto it = m_name_index.find(field_name);
		assert(it != m_name_index.end());
		int idx = it->second;
		if (idx >= m_tmp_records.size())
		{
			field.val = "";
		}
		else
		{
			field.val = m_tmp_records[idx];
		}

		return field;
	}
	csv_file_reader& csv_file_reader::operator >> (int &val)
	{
		val = std::atoi(m_tmp_records[m_cur_idx].c_str());
		m_cur_idx++;
		return (*this);
	}
	csv_file_reader& csv_file_reader::operator >> (double &val)
	{
		val = std::atof(m_tmp_records[m_cur_idx].c_str());
		m_cur_idx++;
		return (*this);
	}
	csv_file_reader& csv_file_reader::operator >> (string &val)
	{
		val = m_tmp_records[m_cur_idx];
		m_cur_idx++;
		return (*this);
	}
	csv_file_reader& csv_file_reader::operator >> (char* val)
	{
		string s = m_tmp_records[m_cur_idx];
		strcpy(val, s.data());
		m_cur_idx++;

		return (*this);
	}
}
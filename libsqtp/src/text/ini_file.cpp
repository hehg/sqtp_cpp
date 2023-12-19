#include "text/ini_file.h"
#include <fstream>
#include <iostream>
namespace sq
{
    bool ini_file::open(const std::string &file_path)
    {

        std::ifstream file(file_path);
        if (!file.is_open())
        {
            std::cerr << "无法打开文件： " << file_path << std::endl;
            return false;
        }

        std::string line;
        std::string current_section;
        while (std::getline(file, line))
        {
            // 去除行首和行尾的空白字符
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // 跳过空行和注释行
            if (line.empty() || line[0] == ';')
            {
                continue;
            }

            // 处理节名
            if (line[0] == '[' && line[line.length() - 1] == ']')
            {
                current_section = line.substr(1, line.length() - 2);
                m_ini_data[current_section] = std::map<std::string, std::string>();
            }
            else
            {
                // 处理键值对
                size_t pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    m_ini_data[current_section][key] = value;
                }
            }
        }

        file.close();
        return true;
    }
    bool ini_file::get_value(const char* section, const char* key, std::string& value)
    {
        if (m_ini_data.find(section) == m_ini_data.end())
        {
            return false;
        }

        if (m_ini_data[section].find(key) == m_ini_data[section].end())
        {
            return false;
        }

        value = m_ini_data[section][key];
        return true;
    }
}
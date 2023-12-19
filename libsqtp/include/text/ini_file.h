#pragma once
#include <map>
#include <string>
using namespace std;
namespace sq
{
    class ini_file
    {
    public:
        ini_file();
        ~ini_file();
        bool open(const std::string& file_path);
        bool get_value(const char* section, const char* key, std::string& value);

    private:
        std::map<std::string, std::map<std::string, std::string>> m_ini_data;
        
    };
}
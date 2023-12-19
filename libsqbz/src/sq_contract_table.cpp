#include "sq_contract_table.h"
#include "text/kv_parser.h"
#include <fstream>

namespace sq {
    /**
     * 从文件加载数据
    */
    bool sq_contract_table::load_from_text(const char* table_name)
    {
        std::ifstream file(table_name);
        if (!file.is_open())
        {
            std::cerr << "无法打开文件： " << table_name << std::endl;
            return false;
        }

        std::string line;
        kv_parser kv;
        while (std::getline(file, line))
        {
            
            // 跳过空行和注释行
            if (line.empty() || line[0] == ';')
            {
                continue;
            }
            kv.parser(line);

            //读取合约内容
            sq_contract info;
            info.from_string(line.c_str());
            std::string key=info.key();
            add(key,info);
            
        }

        file.close();
        return true;
    }
}
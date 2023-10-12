#pragma once
#include "sq_define.h"
#include "sq_macro.h"
#include "text/sq_config.h"
#include <string>

namespace sq
{
    class sq_app
    {

    public:
        sq_app();
        virtual ~sq_app();

        //获取app文件的路径
		const char* get_app_path();
		const char* get_app_name();
		const char* get_app_folder();

        //启动信号捕获
		virtual void init(int argc, char **argv);
        virtual void run(){
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
        };

        //读取配置信息
		std::string get_config_string(const char* key, const char* default_val="");
		
		int    get_config_int(const char* key, int default_val=0);
		
		double get_config_double(const char*key, double default_val = 0);
		
		int   get_config_array(const char*key, std::vector<std::string> &values);
		
		int   get_config_array_int(const char*key, std::vector<int> &values);
		
		bool get_config_bool(const char*key,bool default_val=true);

        int m_app_id=0;
        std::string m_today;
        std::string m_app_path;
        std::string m_app_name;
        std::string m_app_dir;
        sq_config   m_config;
        std::string m_conf_path="../conf/config.cfg";

    };
    
    
    
}
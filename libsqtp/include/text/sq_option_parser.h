/**
 * 命令行参数
*/
#pragma once
#include <string>
#include <unordered_map>
#include <functional>
using namespace std;
namespace sq {
    class sq_option_parser
    {
        using option_action_fun_t = std::function<void()>;
        struct option_info {
            string short_opt;
            string long_opt;
            string help;
            option_action_fun_t action;
            string value;
            bool   has_set;
        };
    public:
        sq_option_parser();
        ~sq_option_parser();
        /**
         * @brief 增加一个命令行参数设置
        */
        void add_option(string short_opt, string long_opt, string help = "",
            option_action_fun_t act = nullptr, string default_val = "");

        /**
         * @brief 解析命令行
        */
        void parser(int argc, char**argv);

        //是否有此命令参数
        bool has_opt(string key);

        //获取命令行参数的值
        string get_value_string(string key);
		int get_value_int(string key);
		double get_value_double(string key);

    private:
        sq_option_parser::option_info* get_option(string key);

        std::unordered_map<string, option_info> m_options;
        string m_app_name = "";
    };

}
#include "text/sq_option_parser.h"
#include <iostream>
namespace sq {
    sq_option_parser::sq_option_parser()
    {
    }


    sq_option_parser::~sq_option_parser()
    {
    }
    void sq_option_parser::add_option(string short_opt, string long_opt, string help,
        option_action_fun_t act, string default_val)
    {
        option_info info{ short_opt ,long_opt ,help ,act ,default_val,false };
        m_options[short_opt] = info;
    }
    void sq_option_parser::parser(int argc, char**argv)
    {
        m_app_name = argv[0];
        int state = -1;
        string key = "";
        sq_option_parser::option_info*key_opt = nullptr;
        for (int i = 1; i < argc; i++) {
			//std::cout << "cur" << "=" << argv[i] << endl;
            if ((argv[i][0] == '-' &&argv[i][1] == '-')|| (argv[i][0] == '-')) 
			{
                key = argv[i];
			//	std::cout << "key" << "=" << key << endl;
                key_opt = get_option(key);
                if (key_opt) 
				{
					if(key_opt->action) 
						key_opt->action();
					key_opt->has_set = true;
                }
              
                continue;
            }
           
            if (key_opt) {
                key_opt->value = argv[i];
                //std::cout << key << "=" << argv[i] << endl;
                key_opt = nullptr;
            }


        }

    }
    sq_option_parser::option_info* sq_option_parser::get_option(string key)
    {
        auto it = m_options.begin();
        for (; it != m_options.end(); ++it) {

            if (it->second.long_opt == key || it->second.short_opt == key) {
                return &it->second;
            }

        }
        cout << "not support option " << key << endl;
        return nullptr;
    }
    bool sq_option_parser::has_opt(string key)
    {
        option_info* info = get_option(key);
        if (info) {
            return info->has_set;
        }
        return false;
    }
    string sq_option_parser::get_value_string(string key)
    {
        option_info* info = get_option(key);
        return info->value;
    }
    int sq_option_parser::get_value_int(string key)
    {
        option_info* info = get_option(key);
        return std::atoi(info->value.c_str());
    }

	double sq_option_parser::get_value_double(string key)
	{
		option_info* info = get_option(key);
		return std::atof(info->value.c_str());
	}
}
#include "text/xml_config_parser.h"
#include "text/sq_string.h"
namespace sq
{
    xml_config_parser::xml_config_parser()
    {
        m_root = nullptr;
        m_file = nullptr;
    }


    xml_config_parser::~xml_config_parser()
    {
        if (m_file)
            delete m_file;
    }
    void xml_config_parser::load(const char*path)
    {
        m_file = new rapidxml::file<char>(path);

        m_doc.parse<0>(m_file->data());
        m_root = m_doc.first_node();
        assert(m_root);
    }
	
    xml_item* xml_config_parser::get_item(const char* xpath, string&item_name, int index)
    {
        vector<string> split_ret;
		sq_split(xpath, split_ret, '/');
        //string name = split_ret[0];
        xml_item* item = m_root;
        if (item == nullptr)
            return nullptr;
        for (size_t i = 1; i < split_ret.size(); i++)
        {
            string name = split_ret[i];
            item = item->first_node(name.c_str());
            if (item == nullptr)
                return nullptr;
        }
        item_name = item->name();
        return item;
    }

	
    string xml_config_parser::get_item_string_value(const char* xpath, string default_val)
    {
        string name;
        xml_item*item = get_item(xpath, name);
        if (item)
        {
            string str = string(item->value());
            return str;
        }
        else{
            return default_val;
        }
    }
    int xml_config_parser::get_item_int_value(const char* xpath, int default_val)
    {
        string name;
        xml_item*item = get_item(xpath, name);
        if (item)
        {
            string str = string(item->value());
            return atoi(str.c_str());
        }
        else{
            return default_val;
        }
    }
    bool xml_config_parser::get_item_bool_value(const char* xpath, bool default_val)
    {
        string name;
        xml_item*item = get_item(xpath, name);
        if (item)
        {
            string str = string(item->value());
            if (str == "true")
              return true;
            else
              return false;
        }
        else{
            return default_val;
        }
    }
    
    void xml_config_parser::get_all_item_value(const char* xpath, vector<string>& value)
    {
        value.clear();
        string name;
        xml_item*item = get_item(xpath, name);
        while (item)
        {
            string str = string(item->value());
            value.push_back(str);
            item = item->next_sibling(name.c_str());
        }
    }
	void xml_config_parser::get_all_item(const char*xpath, vector<xml_item*>&value)
	{
		value.clear();
		string name;
		xml_item*item = get_item(xpath, name);
		while (item)
		{
			value.push_back(item);
			item = item->next_sibling(name.c_str());
		}
	}

    int    xml_config_parser::get_item_count(const char* xpath)
    {
        string name;
        int count = 0;
        xml_item*item = get_item(xpath, name);
        while (item)
        {
            string str = string(item->value());
            count++;
            item = item->next_sibling(name.c_str());
        }
        return count;
    }
}

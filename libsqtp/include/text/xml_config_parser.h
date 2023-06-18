/**
@file
@brief xml配置文件解析

     基于rapidxml 实现的xml文件解析工具,方便xml配置文件读取


@date  2016-03-10
@author      
@copyright (c) 2016 大连飞创信息技术有限公司. All rights reserved.
*/
#pragma once
#include <string>
#include <vector>
using namespace std;
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"

using namespace rapidxml;

namespace sq
{
	typedef   xml_node<> xml_item;

	class xml_config_parser
    {
    public:
		xml_config_parser();
		~xml_config_parser();

        //加载xml文件
        //@param path xml文件路径
        void load(const char*path);
        //获取xml路径上某个item的值
        //@param xpath item路径 形式如： /par/par1
        //@param item_name [out]  item名称
        //@index 索引,表示获取第几个item的值
		xml_item* get_item(const char* xpath, string&item_name, int index = 0);

        //获取xpath的个数
        int    get_item_count(const char* xpath);

        //获取xpath的值
        string get_item_string_value(const char* xpath, string default_val = "");

        //获取xpath的值
        int get_item_int_value(const char* xpath, int default_val = 0);

        //获取xpath的值
        bool get_item_bool_value(const char* xpath, bool default_val = false);

        //获取所有xpath的值
        void get_all_item_value(const char* xpath, vector<string>& value);

		//获取所有item
		void get_all_item(const char*xpath, vector<xml_item*>&value);
    private:
        xml_item*m_root;
        xml_document<> m_doc;
        rapidxml::file<>* m_file;

    };
}


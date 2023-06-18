/**
Copyright (c) 2003-2015 
@brief 创建对象的工厂，根据名称创建各种对象

@author 
*/

#pragma once
#include "sq_define.h"
#include <unordered_map>
using namespace std;
namespace sq
{

	typedef void*(*create_sqt_object)();

	class sqt_object_factory
	{
	public:
		static sqt_object_factory&instance()
		{
			static sqt_object_factory s;
			return s;
		}
		
		void regist_class(string class_name, create_sqt_object fun)
		{
			m_class_map[class_name] = fun;
		}
		void* create_instance_for_class(string class_name)
		{
			auto it = m_class_map.find(class_name);
			if (it != m_class_map.end())
				return it->second();
			return nullptr;

		}

		sqt_object_factory(){};
		~sqt_object_factory(){};
	public:
		std::unordered_map<string, create_sqt_object> m_class_map;

	};
	//static sqt_object_factory s_factor;

	class dynamic_create
	{
	public:
		dynamic_create(string name, create_sqt_object fun){
			sqt_object_factory::instance().regist_class(name, fun);
		}
		void print(){}
	};
}
#define DECLARE_DYNAMIC_CREATE(classname) public: static void*create_sqt_object(){ return new classname(); }


#define IMPLEMENT_DYNAMIC_CREATE(classname) \
	static const dynamic_create classname##gd = dynamic_create(#classname, classname::create_sqt_object)

	
//#define REGIST_SQP(classname)  dynamic_create classname##gd(#classname, classname::create_sqt_object)

#pragma once
#include <functional>
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;
/**单体测试框架
*/

class unit_test
{
public:
	static unit_test*instance();
private:
	unit_test() {}
	~unit_test() {}
public:
	using unit_test_fun_t = std::function<void()>;

	vector<unit_test_fun_t> m_func_array;
	vector<string> m_func_names;

	void run(const char* name=nullptr)
	{
		for (size_t i = 0; i < m_func_array.size(); i++)
		{
			if (name &&strcmp(name, m_func_names[i].c_str()) == 0)
			{
				m_func_array[i]();
				std::cout << "test " << m_func_names[i].c_str() << " ~finish~" << endl;
			}
			else if(name==nullptr)
			{
				m_func_array[i]();
				std::cout << "test " << m_func_names[i].c_str() << " ~finish~" << endl;
			}

		}

	}
	void push(unit_test_fun_t fun,const char*name)
	{
		m_func_array.push_back(fun);
		m_func_names.push_back(name);
	}
};
#define UT (unit_test::instance())
/*定义TEST*/

#define SQ_TEST(NAME) void NAME();\
class local_fun_class##NAME {\
public:local_fun_class##NAME(){unit_test::instance()->push(NAME,#NAME);}\
}; \
static const local_fun_class##NAME aa##NAME;\
void NAME()



/*定义比较函数*/


#define SQ_EXPECT_EQ(a,b) do{if ((a) != (b)) std::cout<<"Err:["<<a<<"!="<<b<<"] "<<__FILE__<<":"<<__LINE__<<endl;}while(0)
#define SQ_EXPECT_STREQ(a,b) do{if (strcmp(a,b)!=0) std::cout<<"Err:["<<a<<"!="<<b<<"] "<<__FILE__<<":"<<__LINE__<<endl;}while(0)
#define SQ_EXPECT_TRUE(a) do{if(!(a))std::cout<<"Err:"<<#a<<" "<<__FILE__<<":"<<__LINE__<<endl;}while(0)

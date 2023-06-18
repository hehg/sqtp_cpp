#pragma once
#include <stdexcept>
namespace sq
{

	class sq_exception :public std::exception
	{
	public:
		sq_exception(const char*file, int line, const char*msg)
		{
			snprintf(buf, sizeof(buf), "%s:%d|%s\n", file, line, msg);

		}
		virtual char const* what()const noexcept 
		{
			return buf;
		}
		char buf[512];

	};

#define SQ_THROW(msg)  do{sq_exception e(__FILE__,__LINE__,msg);throw(e);}while(0)

}

/**
Copyright (c) 2003-2015 
@brief application基类，实现一些应用的基本功能


@author 
*/
#ifndef SQ_APP_BASE_H
#define SQ_APP_BASE_H
#include "sq_frame.h"
#include "sq_struct_def.h"
#include "log/sq_logger.h"
#include <string>
#include <vector>

using namespace std;
using namespace sq;

#ifdef WINDOWS
	#ifdef USE_SQ_DLL_EXPORT
	#define SQ_DLL_EXPORT __declspec(dllexport)
	#else
	#define	SQ_DLL_EXPORT __declspec(dllimport)
	#endif
#else
#define SQ_DLL_EXPORT
#endif

namespace sq
{
	/** app 基类,应用相关功能封装，与交易无关
	 */
 	class SQ_DLL_EXPORT sq_app_base
	{
	public:
		sq_app_base();
		~sq_app_base();
	public:
		//获取app文件的路径
		const char* get_app_path();
		const char* get_app_name();
		const char* get_app_folder();

        //启动信号捕获
        void   capture_signal();
		virtual void init(int argc, char **argv);
        virtual void run();
		/**程序退出时调用
        @param signal 导致退出的信号
        */
		virtual void on_close(int signal) { }
        virtual int on_signal(int sig){return 0;}
		virtual void show_help();

	};

}
#endif
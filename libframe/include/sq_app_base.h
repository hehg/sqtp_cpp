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
	/** app 基类
	 */
 	class SQ_DLL_EXPORT sq_app_base
	{
	public:
		sq_app_base();
		~sq_app_base();
		void on_frame_msg(int tid,char*data,int size);
	public:
		//获取app文件的路径
		const char* get_app_path();
		const char* get_app_name();
		const char* get_app_folder();

        //启动信号捕获
        void   capture_signal();
		virtual void init(int argc, char **argv);
        virtual void run();
		virtual void on_trade_msg(int tid,char*data,int size);
		virtual void on_market_data(sq::sq_quot*quot){}
		virtual void on_order_state(int64_t local_id,const char*contract,int state,int direction,int offset){}
		virtual void on_order_match(int64_t local_id,const char*contract,double match_price,int match_qty,int direction,int offset){}
		/**程序退出时调用
        @param signal 导致退出的信号
        */
		virtual void on_close(int signal) { }
        virtual int on_signal(int sig){return 0;}
		virtual void show_help();

		//读取配置信息
		std::string get_config_string(const char* key, const char* default_val="");
		
		int    get_config_int(const char* key, int default_val=0);
		
		double get_config_double(const char*key, double default_val = 0);
		
		int   get_config_array(const char*key, std::vector<std::string> &values);
		
		int   get_config_array_int(const char*key, std::vector<int> &values);
		
		bool  get_config_bool(const char*key,bool default_val=true);
		
	};

}
#endif
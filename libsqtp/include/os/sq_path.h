/**
Copyright (c) 2003-2015 
@brief 操作系统路径


@author 
*/
#pragma once
#include <string>
#include <vector>
#include <list>
using namespace std;

namespace sq
{
	/**
		生成路径
		*/
	inline void make_path(string& ss) {}
	template<typename T, typename...Ts>
	void make_path(string& ss, T dir, Ts... other)
	{
		//cout<<sizeof...(Ts)<<std::endl;
		ss += dir;
		if (sizeof...(Ts) > 0) {
			ss += "/";
		}

		make_path(ss, other...);
	}

	class sq_path
	{
	public:
		sq_path(const char* path);
		~sq_path();

		inline const char* get_name(){ return m_name; }
		inline const char* get_ext(){ return m_ext; }
		inline const char* get_dir(){ return m_dir; }
		inline const char* get_driver(){ return m_driver; }

		
		//static string  make_path(const char*str1, ...);
		
		static bool mkdir(const char*dir);
		static bool rmdir(const char*dir);

		static bool exist(const char*path); //文件是否存在
		/*@brief查找目录下的所有文件
		  @param dir-目录
		  */
		static vector<string> get_files(const char* dir, const string &postfix);

		static bool is_folder(const char*path);
		static bool is_file(const char*path);
		static bool is_hide(const char*path);

		static void walk_dir(const char *dir_path,std::vector<string>& files,bool revers=false);

	private:
		char m_path[1024];
		char m_driver[10];
		char m_dir[512];
		char m_name[512];
		char m_ext[512];
	};
}

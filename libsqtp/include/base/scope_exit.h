/**
Copyright (c) 2003-2015 
@brief 作用域退出时，执行某个回调函数


@author 
*/

#pragma once
/**作用域退出时，执行某个回调函数
*/
namespace sq
{
	template <typename T>
	struct scope_exit
	{
		scope_exit(T &&t) : t_{ std::move(t) } {}
		~scope_exit() { t_(); }
		T t_;
	};

	template <typename T>
	scope_exit<T> make_scope_exit(T &&t) {return scope_exit<T>{std::move(t)};}
}
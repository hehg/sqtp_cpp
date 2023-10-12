/**
Copyright (c) 2003-2015
@brief 数学函数相关的操作封装


@author
*/

#pragma once
#include "sq_define.h"
#include <random>
namespace sq
{
	class math_util
	{
	public:
		/// 是否为2的幂
		static inline bool is_power2(uint32_t n)
		{
			return !(n & (n - 1));
		}

		static int next_power2(int v);

		static inline int double_compare(double a, double b, double accuracy = 0.00001)
		{
			double d = a - b;
			if (d > 0)
			{
				return (d > accuracy) ? 1 : 0;
			}
			else if (d == 0)
			{
				return 0;
			}
			else
			{
				return (d < -accuracy) ? -1 : 0;
			}
		}
		// static int double_compare(double s1, double s2);
		// 判断整数的个数
		template <typename T = int>
		int digits(T x)
		{
			int a = x == 0 ? 0 : 1;
			return (int)(a * (T)log10(abs(x * 1.0)) + 1);
		}
		// 产生随机数
		static int random_int(int min, int max)
		{
			static uniform_int_distribution<unsigned> u(min, max);
			static default_random_engine e(time(0));
			int a = u(e);
			return a;
		}
	};
}

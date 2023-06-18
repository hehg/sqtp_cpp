#include "math/math_util.h"

int  static find_last_set(int x)
{
	unsigned long mask = 0x80000000;
	int i = 0;
	for (; i < 32; i++)
	{
		unsigned long v = (x << i);
		if ((mask&v) != 0)
			break;
	}
	int ret = 0;
	if(x>0)
		ret=32 - i;
	return ret;
}

namespace sq
{
	int math_util::next_power2(int v) {
		return v ? (1ul << find_last_set(v - 1)) : 1;
	}

	/*int math_util::double_compare(double s1, double s2) {
		const double DPREC = 0.00001;
		if (fabs(s1 - s2) < DPREC)
		{
			return 0;
		}
		return (s1 > s2) ? 1 : -1;
	}
	*/
}
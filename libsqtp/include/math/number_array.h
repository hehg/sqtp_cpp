#include "sq_define.h"

namespace sq
{
	/**
	 * 数据数组，提供一些基本功能
	*/
	template<typename T>
	class number_array:public std::vector<T>
	{
		
	public:
		number_array() = default;
		number_array(initializer_list<T> var) :vector<T>(var) {}
		T sum()
		{
			T sum = 0;
			for (size_t i = 0; i < vector<T>::size(); i++)
			{
				sum += vector<T>::operator[](i);
			}
			return sum;
		}
        T avg() {
            return sum() / std::vector<T>::size();
        }
		
		number_array operator*(number_array &other)
		{
			number_array tmp;
			for (size_t i = 0; i < vector<T>::size(); i++)
			{
				//tmp.push_back(vector<T>::at(i)*other.at(i));
			}
			return tmp;
		}
		
	};
}

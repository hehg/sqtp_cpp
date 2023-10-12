#pragma once
#include "ptr_array.h"
#include "sq_define.h"

class sq_mem_row:public sq::ptr_array_item
{
public:
	sq_mem_row(void*data) { m_data = data; }
	void* m_data=nullptr;
};



//带自定义比较函数的索引对象（模板）。
//利用STL中的set来进行快速有效的排序。
template<typename K, typename Compare>
class sq_mem_key
{
public:
	typedef std::set<K, Compare> SET_;
	typedef typename SET_::iterator LP_SET;
	SET_ m_index;
	LP_SET m_current;

public:
	/**构造函数
	*/
	sq_mem_key()
	{
	}

	/**析构函数
	*/
	~sq_mem_key()
	{
		Clear();
	}

	/**清除索引队列
	*/
	void Clear()
	{
		m_index.clear();
	}

	/**获取第一个索引值
	* @param	Record& rt		第一个索引值.
	* @return	bool
	*/
	bool First(K& rt)
	{
		m_current = m_index.begin();
		if (m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}

	/**获取下一个索引值
	* @param	Record& rt		下一个索引值.
	* @return	bool
	*/
	bool Next(K& rt)
	{
		m_current++;
		if (m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}

	/**查找索引值是否存在
	* @param	Record& rt		索引值.
	* @return	bool
	*/
	bool Find(K& rt)
	{
		m_current = m_index.find(rt);
		if (m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}

	/**增加一个索引值
	* @param	Record& rt		索引值.
	*/
	void Add(K &rt)
	{

		m_index.insert(rt);

	}

	/**删除一个索引值
	* @param	Record& rt		索引值.
	*/
	void Erase(K& rt)
	{
		m_index.erase(rt);
	}
};


template <typename Record>
class sq_mem_tb
{
public:
	
protected:
	int push_back(Record*record) {
		sq_mem_row*row=new sq_mem_row(record);
		m_rows.push_back(row);

		int idx= row->get_array_index();
		return idx;
	}
	int size() { return m_rows.size();}
	void erase(int idx) {
		sq_mem_row*row=m_rows.erase(idx);
		delete row;
	}
	Record* get(int idx) {
		Record*r=(Record*)m_rows[idx]->m_data;
		return r;
	}
	sq::ptr_array<sq_mem_row> m_rows;
	
};
template<int count, typename Record, int order>
class sq_int_key_tb:public sq_mem_tb<Record>
{

public:
	struct Int_
	{
		int	k[count];
		int id;
	};
	struct Int_lt
	{
		bool operator()(const Int_ &f1, const Int_ &f2) const
		{
			int r = 0;
			int i;
			int o = 1;

			for (i = 0; i<count; i++)
			{
				if ((o & order) == 0)
				{
					r = f1.k[i] - f2.k[i];
				}
				else
				{
					r = f2.k[i] - f1.k[i];
				}
				o = o << 1;
				if (r != 0)
				{
					break;
				}
			}
			return r < 0;
		}
	};
	//容器内部的索引对象
	sq_mem_key<Int_, Int_lt> m_index;


	sq_int_key_tb() {}
	~sq_int_key_tb() {}


	void Insert(Record *rt, int k, ...)
	{
		Int_ a_;
		va_list	intp;
		int i, id;
	

		id=this->push_back(rt);

		a_.id = id;
		a_.k[0] = k;
		va_start(intp, k);
		for (i = 0; i<count - 1; i++)
		{
			a_.k[i + 1] = va_arg(intp, int);
		}
		va_end(intp);
		m_index.Add(a_);
		
		return;
	}

	

	/**查询容器中对象
	* @param	Record &rt		查询后获得的对象.
	* @param	int k			索引项.
	*/
	Record* Find(int k, ...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;

		a_.k[0] = k;
		va_start(intp, k);
		for (i = 0; i<count - 1; i++)
		{
			a_.k[i + 1] = va_arg(intp, int);
		}
		va_end(intp);
		bret_ = m_index.Find(a_);
		if (bret_ == true) {
			this->get(a_.id);
		}
			
		return nullptr;
	}

	/**查询容器索引项对应的对象是否存在
	* @param	int k			索引项.
	*/
	bool Exist(int k, ...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;

		a_.k[0] = k;
		va_start(intp, k);
		for (i = 0; i<count - 1; i++)
		{
			a_.k[i + 1] = va_arg(intp, int);
		}
		va_end(intp);
		bret_ = m_index.Find(a_);
		return bret_;
	}

	/**取得本容器中排序第一的对象
	* @param	Record &rt			容器中的对象.
	*/
	Record* First()
	{
		Int_ a_;

		if (m_index.First(a_) == true)
		{
			return this->get(a_.id);
		}
		else
		{
			return nullptr;
		}
	}

	/**取得本容器中排序的下一个的对象
	* @param	Record &rt			容器中的对象.
	*/
	Record* Next()
	{
		Int_ a_;

		if (m_index.Next(a_) == true)
		{
			return this->get(a_.id);
		}
		else
		{
			return nullptr;
		}
	}

	/**删除本容器中索引项对应的对象
	* @param	int k			索引项.
	*/
	void Delete(int k, ...)
	{
		Int_ a_;
		va_list	intp;
		int i;
		

		a_.k[0] = k;
		va_start(intp, k);
		for (i = 0; i<count - 1; i++)
		{
			a_.k[i + 1] = va_arg(intp, int);
		}
		va_end(intp);
		if (m_index.Find(a_) == true)
		{
			m_index.Erase(a_);
			this->erase(a_.id);
		}
		return;
	}
};
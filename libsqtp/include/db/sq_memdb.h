#pragma once
#include "ptr_array.h"
#include "sq_define.h"

class sq_mem_row:public sq::ptr_array_item
{
public:
	sq_mem_row(void*data) { m_data = data; }
	void* m_data=nullptr;
};



//���Զ���ȽϺ�������������ģ�壩��
//����STL�е�set�����п�����Ч������
template<typename K, typename Compare>
class sq_mem_key
{
public:
	typedef std::set<K, Compare> SET_;
	typedef typename SET_::iterator LP_SET;
	SET_ m_index;
	LP_SET m_current;

public:
	/**���캯��
	*/
	sq_mem_key()
	{
	}

	/**��������
	*/
	~sq_mem_key()
	{
		Clear();
	}

	/**�����������
	*/
	void Clear()
	{
		m_index.clear();
	}

	/**��ȡ��һ������ֵ
	* @param	Record& rt		��һ������ֵ.
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

	/**��ȡ��һ������ֵ
	* @param	Record& rt		��һ������ֵ.
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

	/**��������ֵ�Ƿ����
	* @param	Record& rt		����ֵ.
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

	/**����һ������ֵ
	* @param	Record& rt		����ֵ.
	*/
	void Add(K &rt)
	{

		m_index.insert(rt);

	}

	/**ɾ��һ������ֵ
	* @param	Record& rt		����ֵ.
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
	//�����ڲ�����������
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

	

	/**��ѯ�����ж���
	* @param	Record &rt		��ѯ���õĶ���.
	* @param	int k			������.
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

	/**��ѯ�����������Ӧ�Ķ����Ƿ����
	* @param	int k			������.
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

	/**ȡ�ñ������������һ�Ķ���
	* @param	Record &rt			�����еĶ���.
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

	/**ȡ�ñ��������������һ���Ķ���
	* @param	Record &rt			�����еĶ���.
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

	/**ɾ�����������������Ӧ�Ķ���
	* @param	int k			������.
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
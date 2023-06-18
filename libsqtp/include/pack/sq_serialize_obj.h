#pragma once

/**这个类实现了将一个对象序列化到一个文件上
*/
namespace sq
{
	template<class  T>
	class sq_serialize_obj
	{
	public:

		/**构造函数
		* @param state 对象的应用
		*/

		sq_serialize_obj(T &state)
		{
			m_fp = NULL;
			m_obj = state;
		}

		/**构造函数，初始化存贮对象
		*/

		sq_serialize_obj()
		{
			m_fp = NULL;
			memset(&m_obj, 0, sizeof(T));
		}

		/**析构函数，关闭文件
		*/

		~sq_serialize_obj()
		{
			close();
		}

		/**打开这个文件，或读取文件的内容
		* @param filename 文件名
		* @return true 成功
		* @return true 失败
		*/

		bool open(char *filename)
		{
			m_fp = fopen(filename, "r+b");
			if (m_fp == NULL) {
				m_fp = fopen(filename, "wb");
				if (m_fp == NULL) {
					return false;
				}
				return Write();
			}
			else {
				return Read();
			}
			return true;
		}

		/**关闭文件
		*/

		void close()
		{
			if (m_fp != NULL) {
				fclose(m_fp);
				m_fp = NULL;
			}
		}

		/**更新在文件中的对象
		* @param pState 这个对象的指针
		* @return true 成功
		* @return true 失败
		*/

		bool update(T *pState)
		{
			m_obj = *pState;
			return Write();
		}

		/**获得这个对象
		* @return 返回这个对象的指针
		*/

		inline  T *get()
		{
			return &m_obj;
		}

	protected:
		/**从文件中读取对象
		* @return true 成功
		* @return true 失败
		*/

		bool read()
		{
			if (m_fp == NULL)
				return false;
			if (fread(&m_obj, sizeof(T), 1, m_fp) != 1) {
				return false;
			}
			return true;
		}

		/**把对象写入文件中
		* @return true 成功
		* @return true 失败
		*/

		bool write()
		{
			if (m_fp == NULL)
				return false;
			fseek(m_fp, 0, SEEK_SET);
			if (fwrite(&m_obj, sizeof(T), 1, m_fp) != 1) {
				return false;
			}
			fflush(m_fp);
			return true;
		}

	protected:
		FILE * m_fp;	//这个文件的指针 
		T m_obj;	// 要写入文件的对象 
	};

}
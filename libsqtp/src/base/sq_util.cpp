#include "base/sq_util.h"
#include "sq_define.h"
#include <fstream>
//#include <iconv.h>

namespace sq
{
	
	 void * sq_util::aligned_malloc(
		size_t size,       ///< Size of the requested memory allocation
		size_t align   ///< The alignment value, which must be an integer power of 2
	)
	{
		#if  defined(WINDOWS)
			return ::_aligned_malloc(size, align);
		#elif defined(Cygwin)|| defined(MAC)
			return malloc(size);
		#else
		   void * pMem;
			return ::posix_memalign(&pMem, align, size) == 0 ? pMem : nullptr;
		#endif


	}

	/// Frees a block of memory that was allocated with aligned_malloc.
	 void sq_util::aligned_free(
		void * mem   ///< A pointer to the memory block that was returned to the aligned_malloc function
	)
	{
	  #if  defined(WINDOWS)
			::_aligned_free(mem);
		#elif defined(Cygwin)|| defined(MAC)
			::free(mem);
		#else
		  	::free(mem);
		#endif


		
	}

	
	 string sq_util::get_env_val(string name)
	 {
#ifdef WINDOWS
		 size_t ret_size = 0;
		 char *buf = nullptr;

		 getenv_s(&ret_size, nullptr, 0, name.c_str());
		 if (ret_size)
		 {
			 buf = (char*)malloc(ret_size * sizeof(char));
			 if (buf)
			 {
				 getenv_s(&ret_size, buf, ret_size, name.c_str());
			 }
		 }
		 if (buf) {
			 string str(buf);
			 free(buf);
			 return str;
		 }
		 else
		 {
			 return "";
		 }

#else
		 const char *val = getenv(name.c_str());
		 /* not found */
		 if (NULL == val) {
			 return "";
		 }
		 return string(val);
#endif

	 }


	 std::string sq_util::check_file_format(const char *path)
	 {
		 using namespace std;
		 ifstream fin(path, ios::binary);
		 unsigned char s2;
		 fin.read((char *)&s2, sizeof(s2)); //读取第一个字节，然后左移8位
		 uint32_t p = s2 << 8;
		 fin.read((char *)&s2, sizeof(s2)); //读取第二个字节
		 p |= s2;

		 string code;

		 switch (p) //判断文本前两个字节
		 {
		 case 0xfffe: //65534
			 code = "UTF-16 LE";
			 break;
		 case 0xfeff: //65279
			 code = "UTF-16 BE";
			 break;
		 case 0xefbb: //61371
			 code = "UTF-8-BOM";
			 break;
		 default:
			 code = "ANSI";
		 }

		 fin.close();
		 return code;
	 }

	  int sq_util::conver_charset(const char*from_charset,const char*to_charset,
		char*inbuf,size_t inlen,char*outbuf,size_t outlen)
		{
			#if 0
           char**pin=&inbuf;
		   char**pout=&outbuf;
		   iconv_t cd= iconv_open(from_charset,to_charset);
		   if(cd==(iconv_t)-1){
			   return -1;
		   }
		   size_t ret=iconv(cd,pin,&inlen,pout,&outlen);
		   iconv_close(cd);
		   return ret;
		   #endif
		   return -1;
		}

}

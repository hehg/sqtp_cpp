#pragma once
#include <stdint.h>
#include <stdlib.h>

#if 0
    void* operator new(size_t size,const char*file,int line)
    {
      void*d =malloc(size);
    // printf("use my new function\n");
      return d;
    }
    void operator delete(void*p) throw()
    {
      //printf("use my delete function\n");
      free(p);
    }

    #ifdef new
    #undef new
    #define new new(__FILE__,__LINE__)
    #else
    #define new new(__FILE__,__LINE__)
    #endif
#endif 
/**
Copyright (c) 2003-2015 
@brief

@author 
*/
#pragma once
#include "sq_define.h"
namespace sq {

    class sq_load_library {
    public:
        bool load(const char*path);
        void close();
        void* get_func(const char* fun_name);
    private:
#ifdef WINDOWS
        HINSTANCE   m_handle = { NULL };
#else
        void *   m_handle={nullptr};
#endif
    };
}

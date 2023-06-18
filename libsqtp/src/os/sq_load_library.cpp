#include "os/sq_load_library.h"
#ifdef WINDOWS
#include <Winbase.h>
#else
#include <dlfcn.h>
#endif

namespace sq {
    bool sq_load_library::load(const char*path)
    {
#ifdef WINDOWS
        m_handle = LoadLibrary(path);
        if (!m_handle) {
            return false;
        }
#else
        m_handle= dlopen(path, RTLD_NOW | RTLD_GLOBAL);
        if (!m_handle) {
            printf("dlopen error,msg=%s\n",dlerror());
            return false;
        }
#endif // 
        return true;
    }
    void* sq_load_library::get_func(const char* fun_name)
    {
#ifdef WINDOWS
       return GetProcAddress(m_handle, fun_name);
#else
       return dlsym(m_handle, fun_name);
#endif
    }
    void sq_load_library::close()
    {
#ifdef WINDOWS
        FreeLibrary(m_handle);
#else
        dlclose(m_handle);
#endif
    }
}
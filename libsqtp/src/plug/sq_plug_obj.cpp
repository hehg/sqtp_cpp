#include "plug/sq_plug_obj.h"
#include "os/sq_load_library.h"
#include <assert.h>
#include "error/sq_err.h"
#include "log/sq_logger.h"
using namespace sq;
plug_obj::~plug_obj()
{
	m_load_lib.close();
}

int plug_obj::load(const char *path)
{
	bool ret=m_load_lib.load(path);
 
    if (ret==false)
    {
        log_fata("load dll {} error\n",path);
        return 1;
    }
	
	create_func_ = (create_func_t)m_load_lib.get_func("plug_create");
	destory_func_ = (destory_func_t)m_load_lib.get_func("plug_destory");
	open_func_ = (open_func_t)m_load_lib.get_func("plug_open");
	close_func_ = (close_func_t)m_load_lib.get_func("plug_close");
	set_option_func_ = (set_option_func_t)m_load_lib.get_func("plug_set_option");
	get_option_func_ = (get_option_func_t)m_load_lib.get_func("plug_get_option");
	put_func_ = (put_func_t)m_load_lib.get_func("plug_put");


    assert(create_func_);
    assert(destory_func_);
    assert(open_func_);
    assert(close_func_);
    assert(set_option_func_);

    return 0;
}
void plug_obj::plug_create()
{
    assert(create_func_);
    plug_ = create_func_();
}

int plug_obj::plug_destory()
{
    assert(destory_func_);
    return destory_func_(plug_);
}
int plug_obj::plug_open()
{
    assert(open_func_);
    return open_func_(plug_);
}
int plug_obj::plug_close()
{
    assert(close_func_);
    return close_func_(plug_);
}
int plug_obj::plug_set_option(const char *key, void *val)
{
    if (set_option_func_)
    {
        return set_option_func_(plug_, key, val);
    }
    else{
        assert(false);
        return err_not_support;
    }
}
int plug_obj::plug_get_option(const char *key, void *val)
{
    if (get_option_func_)
    {
        return get_option_func_(plug_, key, val);
    }
    else
    {
         assert(false);
        return err_not_support;
    }
}
int plug_obj::plug_put(uint16_t tid, char *data, uint16_t size)
{
    if (put_func_)
    {
        return put_func_(plug_, tid, data, size);
    }
    else
    {
         assert(false);
        return err_not_support;
    }
}

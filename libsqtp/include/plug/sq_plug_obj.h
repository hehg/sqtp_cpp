#pragma once
#include "plug/sq_plug.h"
#include "os/sq_load_library.h"
//插件加载器
class plug_obj
{
public:
    plug_obj(){};
    ~plug_obj();
    int load(const char*path);

    void plug_create();
    int  plug_destory();
    int  plug_open();
    int  plug_close();
    int  plug_set_option(const char*key,void*val);
    int  plug_get_option(const char*key,void*val);
    int  plug_put(uint16_t,char*,uint16_t size);
private:
    create_func_t create_func_ = nullptr;
    destory_func_t destory_func_ = nullptr;
    open_func_t open_func_ = nullptr;
    close_func_t close_func_ = nullptr;
    set_option_func_t set_option_func_ = nullptr;
    get_option_func_t get_option_func_ = nullptr;
    put_func_t        put_func_=nullptr;
    void*handler_=nullptr;
	sq::sq_load_library  m_load_lib;

public:
    int plug_id_=0;
    PLUG* plug_=nullptr;
};
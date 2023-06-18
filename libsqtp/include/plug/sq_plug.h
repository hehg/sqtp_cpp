#pragma once
#include <stdint.h>

#ifdef WINDOWS
#ifdef USE_SQ_DLL_EXPORT
#define SQ_DLL_EXPORT __declspec(dllexport)
#else
#define	SQ_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define SQ_DLL_EXPORT
#endif

#define PLUG void

typedef PLUG *(*create_func_t)();
typedef int (*destory_func_t)(PLUG *);
typedef int (*open_func_t)(PLUG *);
typedef int (*close_func_t)(PLUG *);
typedef int (*set_option_func_t)(PLUG *,const char* key,void*param);
typedef int (*get_option_func_t)(PLUG *,const char* key,void*param);
typedef int (*put_func_t)(PLUG *,uint16_t tid, char* data,uint16_t size);
typedef void (*plug_callback_func_t)(uint16_t tid,char*data,uint16_t size,void*param,int plug_id);

// 插件参数
#define PLUG_OPT_id "id"
#define PLUG_OPT_callback "callback"
#define PLUG_OPT_callback_param "callback_param"
#define PLUG_OPT_plug_name "name"

extern "C"
{
	SQ_DLL_EXPORT PLUG* plug_create();
	SQ_DLL_EXPORT int plug_destory(PLUG*);
	SQ_DLL_EXPORT int plug_open(PLUG*);
	SQ_DLL_EXPORT int plug_close(PLUG*);
	SQ_DLL_EXPORT int plug_set_option(PLUG*,const char*key ,void*val);
	SQ_DLL_EXPORT int plug_get_option(PLUG*,const char*key ,void*val);
	SQ_DLL_EXPORT int plug_put(PLUG*,uint16_t tid, char* data,uint16_t size);
}
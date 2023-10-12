#include "plug/sq_plug.h"
#include "bz_stock_fe.h"
using namespace std;

/**
 * 行情转发插件，通过组播或udp的方式
*/
//=====接口实现=======
PLUG *plug_create()
{
    using namespace sq_plug;
    bz_stock_fe *plug = new bz_stock_fe();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (bz_stock_fe *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((bz_stock_fe *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((bz_stock_fe *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
    using namespace sq_plug;
   return ((bz_stock_fe *)plug)->set_option(k, v);
}
 int plug_put(PLUG*plug,uint16_t tid, char* data,uint16_t size)
 {
    using namespace sq_plug;
   return ((bz_stock_fe *)plug)->put(tid,data,size);
 }

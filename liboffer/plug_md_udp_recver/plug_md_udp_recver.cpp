#include "plug/sq_plug.h"
#include "md_udp_recver.h"
using namespace std;

/**
 * 组播或者udp方式接收行情
*/
//=====接口实现=======
PLUG *plug_create()
{
    using namespace sq_plug;
    md_udp_recver *plug = new md_udp_recver();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (md_udp_recver *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((md_udp_recver *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((md_udp_recver *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
    using namespace sq_plug;
   return ((md_udp_recver *)plug)->set_option(k, v);
}
 int plug_put(PLUG*plug,uint16_t tid, char* data,uint16_t size)
 {
    using namespace sq_plug;
   return ((md_udp_recver *)plug)->put(tid,data,size);
 }

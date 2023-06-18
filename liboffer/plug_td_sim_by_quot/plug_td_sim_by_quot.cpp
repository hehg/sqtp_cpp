#include "plug/sq_plug.h"
#include "td_sim_by_quot.h"
using namespace std;

//=====接口实现=======

PLUG *plug_create()
{
    using namespace sq_plug;
    td_sim_by_quot *plug = new td_sim_by_quot();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (td_sim_by_quot *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((td_sim_by_quot *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((td_sim_by_quot *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
   using namespace sq_plug;
   return ((td_sim_by_quot *)plug)->set_option(k, v);
}
 int plug_put(PLUG*plug,uint16_t tid, char* data,uint16_t size)
 {
    using namespace sq_plug;
   return ((td_sim_by_quot *)plug)->put(tid,data,size);
 }
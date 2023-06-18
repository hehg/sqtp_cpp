#include "plug/sq_plug.h"
#include "td_sim_driver.h"
using namespace std;

//=====接口实现=======

PLUG *plug_create()
{
    using namespace sq_plug;
    td_sim_driver *plug = new td_sim_driver();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (td_sim_driver *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((td_sim_driver *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((td_sim_driver *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
   using namespace sq_plug;
   return ((td_sim_driver *)plug)->set_option(k, v);
}
 int plug_put(PLUG*plug,uint16_t tid, char* data,uint16_t size)
 {
    using namespace sq_plug;
   return ((td_sim_driver *)plug)->put(tid,data,size);
 }
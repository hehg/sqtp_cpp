#include "plug/sq_plug.h"
#include "plug_bz_risk_imp.h"
using namespace std;

/**
 * 行情转发插件，通过组播或udp的方式
*/
//=====接口实现=======
PLUG *plug_create()
{
    using namespace sq_plug;
    plug_bz_risk_imp *plug = new plug_bz_risk_imp();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (plug_bz_risk_imp *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((plug_bz_risk_imp *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((plug_bz_risk_imp *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
    using namespace sq_plug;
   return ((plug_bz_risk_imp *)plug)->set_option(k, v);
}
 int plug_put(PLUG*plug,uint16_t tid, char* data,uint16_t size)
 {
    using namespace sq_plug;
   return ((plug_bz_risk_imp *)plug)->put(tid,data,size);
 }

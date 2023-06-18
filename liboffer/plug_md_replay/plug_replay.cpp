#include "plug/sq_plug.h"
#include "replay_quot.h"
using namespace std;

//=====接口实现=======
PLUG *plug_create()
{
    using namespace sq_plug;
    replay_quot *plug = new replay_quot();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (replay_quot *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    ((replay_quot *)plug)->open();
    return 0;
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    ((replay_quot *)plug)->close();
    return 0;
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
    using namespace sq_plug;
   return ((replay_quot *)plug)->set_option(k, v);
}


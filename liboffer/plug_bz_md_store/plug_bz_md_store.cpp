#include "plug/sq_plug.h"
#include "store_quot.h"
using namespace std;

//=====接口实现=======
PLUG *plug_create()
{
    using namespace sq_plug;
    store_quot *plug = new store_quot();
    return plug;
}

int plug_destory(PLUG *plug)
{
    using namespace sq_plug;
    delete (store_quot *)plug;
    return 0;
}
int plug_open(PLUG *plug)
{
    using namespace sq_plug;
    return ((store_quot *)plug)->open();
 
}
int plug_close(PLUG *plug)
{
    using namespace sq_plug;
    return ((store_quot *)plug)->close();
   
}

int plug_set_option(PLUG *plug, const char *k, void *v)
{
    using namespace sq_plug;
   return ((store_quot *)plug)->set_option(k, v);
}

int plug_put(PLUG*plug, uint16_t tid, char* data, uint16_t size)
{
	using namespace sq_plug;
	return ((store_quot *)plug)->put(tid, data, size);
}
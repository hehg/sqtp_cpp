#include "md_server.h"
#include "http/ws_server.h"
#include "log/sq_logger.h"
#include "error/sq_err.h"
#include "sq_macro.h"
using namespace sq;

md_server::md_server()
{
   m_id=ID::md_server;
}
md_server::~md_server()
{
}
int md_server::open(const char *xml_path)
{
    
    //检查是否启动
    log_info("md_server open ok\n");
    return ok;
}
int md_server::stop()
{
    return ok;
}
#include "app.h"
#include <string>
#include "log/sq_logger.h"
using namespace std;
namespace bus
{
    app the_app;
    void app::init(int argc, char **argv)
    {
        sq::sq_app::init(argc, argv);
        SQ_LOG_LEVEL(log_debug);
    }
    void app::run()
	{
        std::string listen_addr= get_config_string("listen_addr");
        m_bus_server.open(listen_addr.c_str());
        m_bus_server.m_ws_server->run();
	}
}
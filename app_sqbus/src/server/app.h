#pragma once
#include "sq_app.h"
#include "bus_server.h"

using namespace sq;
namespace bus
{
    class app  :public sq::sq_app
    {
    public:
         void init(int argc, char **argv) override;
         void run()override;
         bus_server m_bus_server;
    };
    extern app the_app;
}
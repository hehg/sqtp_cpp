#include <stdlib.h>
#include "log/sq_logger.h"
#include "../api/sq_bus_api.h"
using namespace bus::api;

// #include "http/ws_client.h"
// #include "pack/sq_package.h"
class cli_rsp : public bus_api_rsp
{
public:
  void on_connected(){
    fmt::print("client connected\n");
  };
  void on_disconnected(){
    fmt::print("client disconnected\n");
  };
  void on_message(const char *data, int size)
  {
    fmt::print("client recv: {}\n",size);
  }
};
int main(int argc, char *argv[])
{
  using namespace sq;
  cli_rsp rsp;
  init_api(&rsp);
  int ret=connect("ws://127.0.0.1:3214");
  if (ret != ok)
  {
    fmt::print("client open failed\n");
    return 0;
  }

  while (true)
  {
    publish(1,"hello",5);
    //client.send_ws_message("hello", 5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
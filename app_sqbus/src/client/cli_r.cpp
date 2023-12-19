#include <stdlib.h>
#include "log/sq_logger.h"
#include "../api/sq_bus_api.h"
using namespace bus::api;
// 客户端接受方
class cli_rsp : public bus_api_rsp
{
public:
  void on_connected(){
    fmt::print("client connected\n");
    subscribe(1);
  };
  void on_disconnected(){
    fmt::print("client disconnected\n");
  };
  virtual void on_subscribe(const char* data,int size)
  {

  }
  void on_topic(unsigned short topic, const char *data, int size)override
  {
    fmt::print("client on_topic: topic={},size={},msg={}\n",topic,size,data);
  }
  void on_message(const char *data, int size)
  {
    fmt::print("client on_message: size={},msg={}\n",size,data);
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
  //subscribe(1);
  
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
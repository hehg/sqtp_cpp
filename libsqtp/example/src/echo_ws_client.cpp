#include "http/ws_client.h"

#include <thread>
using namespace sq;
int main()
{
  ws_client client([](ws_channel *channel, void *msg, int size) -> int
                   {
    char*data = (char*)msg;
    printf("client recv: %s\n",data);
    return size; });

  client.open("ws://127.0.0.1:2341", "/");
  std::thread t([&]()
                { get_default_reactor()->loop(); });
  while (true)
  {
    client.send_ws_message("hello", 5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
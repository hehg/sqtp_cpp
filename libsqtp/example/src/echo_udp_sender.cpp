#include "net/udp_sender.h"
#include "net/reactor.h"
#include <thread>
using namespace sq;
int main()
{
  udp_sender client;

  client.add_channel("udp://127.0.0.1:3456");
  std::thread t([&]()
                { get_default_reactor()->loop(); });
  while (true)
  {
    client.send("hello", 5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
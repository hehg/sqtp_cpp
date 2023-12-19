#include "net/tcp_client.h"
#include "net/reactor.h"
#include <thread>
using namespace sq;
int main()
{
  tcp_client client([](tcp_channel *channel, void *msg, int size) -> int
                    {
    char*data = (char*)msg;
    printf("client recv: %s\n",data);
    return size; });

  client.add_channel("tcp://127.0.0.1:1234");
  std::thread t([&]()
                { get_default_reactor()->loop(); });
  while (true)
  {
    client.send("hello", 5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
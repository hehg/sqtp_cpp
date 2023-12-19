#include "net/tcp_server.h"
#include <thread>
using namespace sq;
int main()
{
  tcp_server server(
      [](tcp_channel *handler, void *msg, int size) -> int
      {
        char *data = (char *)msg;
        printf("server recv: %s\n", data);
        int ret = handler->send_data(data, size);
        if (ret!=ok)
        {
          printf("send data failed\n");
        }

        return size;
      });

  server.open("tcp://0.0.0.0:1234");
  server.run();
  return 0;
}
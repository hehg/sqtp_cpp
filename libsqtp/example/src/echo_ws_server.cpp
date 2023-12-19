#include "http/ws_server.h"
#include <thread>

using namespace sq;
int main()
{
    ws_server server(
      [](ws_channel *handler, void *msg, int size) -> int
      {
        char *data = (char *)msg;
        printf("server recv: %s\n", data);
        int ret=handler->send_ws_package(data, size);
        if (ret!=ok)
        {
          printf("send data failed\n");
        }

        return size;
      },nullptr,nullptr,nullptr);

  server.open("tcp://0.0.0.0:2341");
  server.run();
  return 0;
}
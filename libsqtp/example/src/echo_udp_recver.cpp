#include "net/udp_recver.h"
#include <thread>
#include "error/sq_err.h"

using namespace sq;
int main()
{
  udp_recver recver(
      [](udp_channel *handler, void *msg, int size) -> int
      {
        char *data = (char *)msg;
        printf("server recv: %s\n", data);
        
        return size;
      });

  recver.add_channel("udp://127.0.0.1:3456");
  reactor_run_forever();
  return 0;
}
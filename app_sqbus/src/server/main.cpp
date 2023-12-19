#include "app.h"
#include <thread>

using namespace sq;
int main(int argc,char**argv)
{
  bus::the_app.init(argc,argv);
  bus::the_app.run();
  return 0;
}
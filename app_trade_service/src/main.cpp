#include <stdlib.h>
#include <stdio.h>
#include "app.h"
using namespace sq;

int main(int argc,char**argv)
{
  the_app.init(argc,argv);
  the_app.run();
}
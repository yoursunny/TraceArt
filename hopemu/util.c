#include "hopemu.h"

void
die(const char* msg)
{
  perror(msg);
  exit(1);
}

#include <stdlib.h>
#include <unistd.h>

int
main()
{
  setuid(0);
  return system("/usr/sbin/service bind9 reload");
}

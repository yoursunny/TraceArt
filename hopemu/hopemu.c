#include "hopemu.h"

int
main()
{
  // open TUN interface
  // while (true) {
  //   read packet from TUN interface
  //   extract destination IPv6 address
  //   split dstaddr as 120-bit prefix and 8-bit suffix
  //   if HopLimit < suffix: reply with ICMPv6 TimeExceeded prefix+HopLimit
  //   else: reply with ICMPv6 PortUnreachable
  // }
  return 0;
}

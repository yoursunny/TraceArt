#include "hopemu.h"
#include <net/if.h>
#include <arpa/inet.h>

#define _LINUX_IN6_H
#include <linux/ipv6.h>
#undef _LINUX_IN6_H

int
processPacket(const char* inPkt, size_t inLen, char* outPkt)
{
  // parse packet
  if (inLen < sizeof(struct ipv6hdr)) {
    fprintf(stderr, "too-short %zu\n", inLen);
    return -1;
  }
  const struct ipv6hdr* ip6 = (struct ipv6hdr*)inPkt;
  if (ip6->version != 6) {
    fprintf(stderr, "wrong-version %u\n", ip6->version);
    return -1;
  }

  char srcAddrP[INET6_ADDRSTRLEN];
  if (inet_ntop(AF_INET6, &ip6->saddr, srcAddrP, sizeof(srcAddrP)) == NULL) {
    fprintf(stderr, "bad-srcaddr\n");
    return -1;
  }
  char dstAddrP[INET6_ADDRSTRLEN];
  if (inet_ntop(AF_INET6, &ip6->daddr, dstAddrP, sizeof(dstAddrP)) == NULL) {
    fprintf(stderr, "bad-dstaddr\n");
    return -1;
  }

  fprintf(stderr, "received %s %s\n", srcAddrP, dstAddrP);

  return 0;
  // split dstaddr as 120-bit prefix and 8-bit suffix
  // if HopLimit < suffix: reply with ICMPv6 TimeExceeded prefix+HopLimit
  // else: reply with ICMPv6 PortUnreachable
}

int
main()
{
  // open TUN interface
  char ifname[IFNAMSIZ];
  strncpy(ifname, "hopemu", sizeof(ifname));
  int fd = tun_open(ifname);

  char inPkt[FRAMESIZE];
  char outPkt[FRAMESIZE];
  while (true) {
    // read packet
    int inLen = tun_read(fd, inPkt);
    int outLen = processPacket(inPkt, inLen, outPkt);
    if (outLen > 0) {
      tun_write(fd, outPkt, outLen);
    }
  }
  return 0;
}

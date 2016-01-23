#include "hopemu.h"
#include <linux/ipv6.h>
#define __USE_KERNEL_IPV6_DEFS
#include <net/if.h>
#include <arpa/inet.h>

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

  // split dstaddr as 120-bit prefix and 8-bit suffix
  const uint8_t* prefix = &ip6->daddr.s6_addr[0]; // 120-bit prefix
  uint8_t suffix = ip6->daddr.s6_addr[15]; // 8-bit suffix

  uint8_t hopLimit = ip6->hop_limit;
  if (hopLimit < suffix) { // packet cannot reach destination
    fprintf(stderr, "time-exceeded %s %s %u\n", srcAddrP, dstAddrP, hopLimit);
  }
  else { // packet reaches destination
    fprintf(stderr, "reach %s %s %u\n", srcAddrP, dstAddrP, hopLimit);
  }

  return 0;
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

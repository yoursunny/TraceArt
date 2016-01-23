#include "hopemu.h"
#include <linux/ipv6.h>
#define __USE_KERNEL_IPV6_DEFS
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/icmp6.h>

#define RFC1071CHECKSUM_NONE (0 ^ 0xffff)
uint16_t
rfc1071checksum(const void* pkt, size_t size, uint16_t lastSum)
{
  if (size % 2 != 0) {
    return 0;
  }

  const uint16_t* p = (const uint16_t*)pkt;
  const uint16_t* end = (const uint16_t*)(pkt + size);

  int sum = lastSum ^ 0xffff;
  while (p < end) {
    sum += (*p);
    sum = (0xffff & sum) + (sum >> 16);
    ++p;
  }
  return (uint16_t)(sum ^ 0xffff);
}

void
updateIcmpChecksum(char* pkt, size_t size)
{
  struct __attribute__ ((__packed__)) Ipv6PseudoHeader
  {
    struct in6_addr saddr;
    struct in6_addr daddr;
    __be32 payload_len;
    __u8 zero[3];
    __u8 nexthdr;
  };

  struct ipv6hdr* ip6 = (struct ipv6hdr*)pkt;
  struct icmp6_hdr* icmp6 = (struct icmp6_hdr*)(pkt + sizeof(struct ipv6hdr));

  struct Ipv6PseudoHeader pseudo;
  memcpy(pseudo.saddr.s6_addr, ip6->saddr.s6_addr, sizeof(pseudo.saddr.s6_addr));
  memcpy(pseudo.daddr.s6_addr, ip6->daddr.s6_addr, sizeof(pseudo.daddr.s6_addr));
  pseudo.payload_len = htobe32(be16toh(ip6->payload_len));
  memset(pseudo.zero, 0, sizeof(pseudo.zero));
  pseudo.nexthdr = IPPROTO_ICMPV6;

  icmp6->icmp6_cksum = 0;
  uint16_t sum = rfc1071checksum(&pseudo, sizeof(pseudo), RFC1071CHECKSUM_NONE);
  sum = rfc1071checksum(icmp6, size - sizeof(struct ipv6hdr), sum);
  icmp6->icmp6_cksum = sum;
}

// make ICMP TimeExceeded or DestinationUnreachable
int
makeIcmpError(char* outPkt, uint8_t type, uint8_t code, const struct in6_addr* srcAddr, const char* inPkt, size_t inLen)
{
  const struct ipv6hdr* inIp6 = (struct ipv6hdr*)inPkt;
  struct ipv6hdr* ip6 = (struct ipv6hdr*)outPkt;
  struct icmp6_hdr* icmp6 = (struct icmp6_hdr*)(outPkt + sizeof(struct ipv6hdr));
  void* payload = (void*)icmp6 + sizeof(struct icmp6_hdr);
  size_t payloadLen = 512;
  if (inLen < payloadLen) {
    payloadLen = inLen;
  }
  if (payloadLen % 2 == 1) {
    --payloadLen;
  }
  size_t outLen = sizeof(struct ipv6hdr) + sizeof(struct icmp6_hdr) + payloadLen;

  ip6->version = 6;
  ip6->priority = 0;
  memset(ip6->flow_lbl, 0, sizeof(ip6->flow_lbl));
  ip6->payload_len = htobe16(outLen - sizeof(struct ipv6hdr));
  ip6->nexthdr = IPPROTO_ICMPV6;
  ip6->hop_limit = 64;
  memcpy(&ip6->saddr.s6_addr, srcAddr, sizeof(ip6->saddr.s6_addr));
  memcpy(&ip6->daddr.s6_addr, &inIp6->saddr.s6_addr, sizeof(ip6->daddr.s6_addr));

  icmp6->icmp6_type = type;
  icmp6->icmp6_code = code;
  icmp6->icmp6_cksum = 0xDDDD;
  icmp6->icmp6_dataun.icmp6_un_data32[0] = 0;

  memcpy(payload, inPkt, payloadLen);
  updateIcmpChecksum(outPkt, outLen);
  return outLen;
}

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
  const uint8_t* prefix = ip6->daddr.s6_addr; // 120-bit prefix
  uint8_t suffix = ip6->daddr.s6_addr[15]; // 8-bit suffix

  uint8_t hopLimit = ip6->hop_limit;
  if (hopLimit < suffix) { // packet cannot reach destination
    fprintf(stderr, "time-exceeded %s %s %u\n", srcAddrP, dstAddrP, hopLimit);
    struct in6_addr icmpSrc;
    memcpy(icmpSrc.s6_addr, prefix, 15);
    icmpSrc.s6_addr[15] = hopLimit;
    return makeIcmpError(outPkt, ICMP6_TIME_EXCEEDED, ICMP6_TIME_EXCEED_TRANSIT, &icmpSrc, inPkt, inLen);
  }
  else { // packet reaches destination
    fprintf(stderr, "reach %s %s %u\n", srcAddrP, dstAddrP, hopLimit);
    return makeIcmpError(outPkt, ICMP6_DST_UNREACH, ICMP6_DST_UNREACH_NOPORT, &ip6->daddr, inPkt, inLen);
  }
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

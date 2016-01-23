#ifndef HOPEMU_TUN_H
#define HOPEMU_TUN_H

#include "common.h"

int
tun_open(char* ifname);

int
tun_read(int fd, char* buffer);

bool
tun_write(int fd, const char* buffer, int len);

#endif //  HOPEMU_TUN_H

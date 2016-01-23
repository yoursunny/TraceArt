#ifndef HOPEMU_H
#define HOPEMU_H

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef int bool;
#define true -1
#define false 0

#define FRAMESIZE 1514

// util
void
die(const char* msg);

// tun
int
tun_open(char* ifname);

int
tun_read(int fd, char* buffer);

bool
tun_write(int fd, const char* buffer, int len);


#endif // HOPEMU_H

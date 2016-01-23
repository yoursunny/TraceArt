all: hopemu

hopemu: *.h *.c
	gcc -std=c99 -o hopemu -D_BSD_SOURCE -g hopemu.c tun.c util.c

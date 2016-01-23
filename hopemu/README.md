# hopemu: emulate network hops inside a TUN interface

This program connects to a TUN interface named `hopemu`, and emulates network hops for IPv6 traceroute.  
`traceroute xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxyy` should show yy hops, which are numbered as `xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xx01` through `xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxyy`.

## Usage

Compile the program:

    make

Create a TUN interface under root privilege:

    ip tuntap add dev hopemu mode tun user <runas-user>
    ip addr add <prefix-96-bit>:0000:0001/96 dev hopemu
    ip link set hopemu up

Run the program under "runas-user":

    ./hopemu

# hopemu: emulate network hops inside a TUN interface

## Usage

Compile the program:

    make

Create a TUN interface under root privilege:

    ip tuntap add dev hopemu mode tun user <runas-user>
    ip addr add <prefix-96-bit>:0000:0001/96 dev hopemu
    ip link set hopemu up

Run the program under "runas-user":

    ./

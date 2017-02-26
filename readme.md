# About
This is prototype of UDP filtering proxy.
It's purpose is DDoS mitigation for UDP application servers.

# Usage
0. Put your UDP application servers inside secure network
1. Place (cheap) boxes with udp_filtering_proxy instances facing clients (world)
2. Each udp_filtering_proxy instance serves only one UDP application server
3. Use more instances to better utilize CPU / network 
4. configure your system limits properly (each client consumes 1 socket FD + 
some memory)


# Implementation
* 2 threads: from world -> server, from server -> world
* 
[Read more](udp_filtering_proxy)

# Prerequisites
* Modern Linux kernel  
* CMake 3.5
* clang / gcc (edit build.sh to select gcc)
* libevent
* libavl
* Enet library (for /test client & server)

# Troubleshooting
If you have troubles linking, try updating your ld path with ```$sudo ldconfig```

# License
GPL Version 3


# Configuration
Edit ```./src/udp_filtering_proxy_config.h.in```

# Usage
```
$./build.sh
$build/bin/udm_proxy
```

# Implementation notes

## Incoming-from-world thread description
0. listens for incoming connections on all interfaces on specified port
1. accepts all packets from outside world
3. for each unique client:
	1. creates struct ClientData
	2. creates unique proxy-to-target-server socket
	3. sends command to Incoming-from-server thread to listen for reply
2. identifies related ClientData
3. ban / un-ban work on ClientData
4. sends packet to target-server via ClientData's proxy-to-target-server socket

## Incoming-from-server thread description
1. listens for incoming replies from server using ClientData's proxy-to-target-server FDs 
3. simply forwards it back to client via outside world FD directly

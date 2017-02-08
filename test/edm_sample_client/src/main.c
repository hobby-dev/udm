#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include "client_config.h"

int main()
{
    printf("Welcome to Enet DDOS Mitigation Client version %d.%d.%d\n",
           EDM_CLIENT_VERSION_MAJOR, EDM_CLIENT_VERSION_MINOR, EDM_CLIENT_VERSION_PATCH);

    if (enet_initialize() != 0)
    {
        perror("enet_initialize()");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost *client;
    client = enet_host_create(NULL /* create a client host */,
                              1 /* allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                              14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
    if (client == NULL)
    {
        perror("enet_host_create()");
        exit(EXIT_FAILURE);
    }

    const ENetVersion version = enet_linked_version();
    printf("Successfully initialized Enet library version %d\nConnecting to server...\n", version);

    ENetAddress address;
    enet_address_set_host(&address, TARGET_ADDRESS);
    address.port = TARGET_PORT;

    ENetPeer * peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        perror("enet_host_connect()");
        exit(EXIT_FAILURE);
    }

        ENetEvent event;
        /* Wait up to 2 seconds for the connection attempt to succeed. */
        if (enet_host_service(client, &event, 2000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            printf("Connection to localhost:12345 succeeded.\n");

            do
            {
            /* Create a reliable packet of size 7 containing "packet\0" */
                ENetPacket *packet = enet_packet_create("packet",
                                                    strlen("packet") + 1,
                                                    ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
            }
            while (enet_host_service(client, &event, 100) >= 0);
        }
        else
        {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset(peer);
            printf("Connection to %s:%d failed\n", TARGET_ADDRESS, TARGET_PORT);
        }

    enet_host_destroy(client);
    return EXIT_SUCCESS;
}


#include <enet/enet.h>
#include <stdio.h>
#include <netdb.h>

int main()
{
    printf("Starting Sample Enet Server....\n");

    if (enet_initialize() != 0)
    {
        perror("enet_initialize()");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    ENetHost *server;
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 31415. */
    address.port = 31415;
    server = enet_host_create(&address /* the address to bind the server host to */,
                              1024      /* allow up to 1024 clients and/or outgoing connections */,
                              2      /* allow up to 2 channels to be used, 0 and 1 */,
                              0      /* assume any amount of incoming bandwidth */,
                              0      /* assume any amount of outgoing bandwidth */);
    if (server == NULL)
    {
        perror("enet_host_create()");
        exit(EXIT_FAILURE);
    }

    const ENetVersion version = enet_linked_version();
    printf("Successfully initialized Enet library version %d\nWaiting for clients...\n", version);

    while (1)
    {
        ENetEvent event;
        /* Wait up to 1000 milliseconds for an event. */
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    char host[NI_MAXHOST], service[NI_MAXSERV];
                    struct sockaddr_in client_addr = {0};
                    client_addr.sin_family = AF_INET;
                    client_addr.sin_addr.s_addr = event.peer->address.host;
                    client_addr.sin_port = event.peer->address.port;
                    const int getname_result = getnameinfo((struct sockaddr *) &client_addr,
                                                           sizeof(client_addr), host, NI_MAXHOST,
                                                           service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
                    if (getname_result == EXIT_SUCCESS)
                        printf("A new client connected from %s:%s\n", host, service);
                    else
                        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(getname_result));

                    /* Store any relevant client information here. */
                    event.peer->data = "Client xxx";
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                           (unsigned int) event.packet->dataLength,
                           event.packet->data,
                           (char *) event.peer->data,
                           event.channelID);
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", (char *) event.peer->data);
                    /* Reset the peer's client information. */
                    event.peer->data = NULL;
                    break;
                default:
                    perror("enet_host_service()");
                    return EXIT_FAILURE;
            }
        }
    }

    enet_host_destroy(server);

    return EXIT_SUCCESS;
}


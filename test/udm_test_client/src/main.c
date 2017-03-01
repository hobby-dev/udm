#include <enet/enet.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "client_config.h"

void print_usage_and_exit(const char *command) {
    fprintf(stderr, "Usage: %s [-s server] [-p port] [-d "
                    "delay (ms)] [-c content] [--verbose]\n",
            command);
    exit(1);
}

int main(int argc, char *argv[]) {
    const char *server_address = DEFAULT_TARGET_ADDRESS;
    uint16_t server_port = DEFAULT_TARGET_PORT;
    uint32_t packet_delay = DEFAULT_PACKET_DELAY;
    const char *packet_content = DEFAULT_PACKET_CONTENT;
    int verbose = 0;

    int c;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"server",  required_argument, 0, 0},
                {"port",    required_argument, 0, 0},
                {"delay",   required_argument, 0, 0},
                {"content", required_argument, 0, 0},
                {"verbose", no_argument,       0, 0},
                // help must be last option in list:
                {"help",    no_argument,       0, 0},
                {0, 0,                         0, 0}
        };

        c = getopt_long_only(argc, argv, "", long_options, &option_index);

        if (c == -1)
            break;

        if (c == 0) {
            int result;
            uint16_t tmp_port;
            uint32_t tmp_delay;
            switch (option_index) {
                case 0: // server
                    server_address = optarg;
                    break;
                case 1: // port
                    result = sscanf(optarg, "%hu", &tmp_port);
                    if (result == 1)
                        server_port = tmp_port;
                    else
                        printf("Wrong --port value %s, default %hu is used\n",
                               optarg, server_port);
                    break;
                case 2: //delay
                    result = sscanf(optarg, "%u", &tmp_delay);
                    if (result == 1)
                        packet_delay = tmp_delay;
                    else
                        printf("Wrong --delay value %s, default %d is used\n",
                               optarg, packet_delay);
                    break;
                case 3: //content
                    packet_content = optarg;
                    break;
                case 4: //verbose
                    verbose = 1;
                    break;
                default:
                    print_usage_and_exit(argv[0]);
            }
        }
    }

    if (optind < argc) {
        printf("Unknown options: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
        print_usage_and_exit(argv[0]);
    }


    if (enet_initialize() != 0) {
        perror("enet_initialize()");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost *client;
    client = enet_host_create(NULL /* create a client host */,
                              1 /* allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              57600 /
                              8 /* 56K modem with 56 Kbps downstream bandwidth */,
                              14400 /
                              8 /* 56K modem with 14 Kbps upstream bandwidth */);
    if (client == NULL) {
        perror("enet_host_create()");
        exit(EXIT_FAILURE);
    }

    const ENetVersion version = enet_linked_version();
    if (verbose)
        printf("Enet library version %d\nConnecting to server...\n",
               version);

    ENetAddress address;
    enet_address_set_host(&address, server_address);
    address.port = server_port;

    ENetPeer *peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
        perror("enet_host_connect()");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    /* Wait up to 2 seconds for the connection attempt to succeed. */
    if (enet_host_service(client, &event, 2000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%hu succeeded.\n", server_address,
               server_port);

        do {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                if (verbose) printf("Disconnected!\n");
                break;
            }

            /* Create a reliable packet of size 7 containing "packet\0" */
            ENetPacket *packet = enet_packet_create(packet_content,
                                                    strlen(packet_content) + 1,
                                                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            if (verbose) printf("Sent packet %s \n", packet_content);
        } while (enet_host_service(client, &event, packet_delay) >= 0);
    } else {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        printf("Connection to %s:%d failed\n", server_address, server_port);
        exit(EXIT_FAILURE);
    }

    enet_host_destroy(client);
    return EXIT_SUCCESS;
}


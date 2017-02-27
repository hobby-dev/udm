#pragma once

#include <arpa/inet.h>
#include <event.h>
#include <stdint.h>
#include <time.h>

struct ClientData {
    struct sockaddr_in client_address;
    socklen_t client_address_len;
    int proxy_to_server_socket_fd;
    uint64_t bits_till_ban;
    struct timespec last_change;
};

struct ClientData *
client_data_get_or_create(struct sockaddr_in *client_address, socklen_t i);

void
client_data_init();

void
client_data_cleanup();


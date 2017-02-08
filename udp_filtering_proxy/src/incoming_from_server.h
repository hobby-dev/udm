#pragma once

#include "client_data.h"

void
start_incoming_from_server(const int incoming_world_socket_fd,
                           struct addrinfo *target_server);

int
add_new_client(struct ClientData *client);
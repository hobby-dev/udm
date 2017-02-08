#pragma once

#include <event.h>

const int
create_world_socket(const uint16_t incoming_port);

struct event *
start_incoming_from_world(struct event_base *event_base,
                          struct addrinfo *target_server,
                          const int world_socket_fd);

void
handle_packet_from_world(evutil_socket_t incoming_socket_fd, short flags,
                         void *arg);
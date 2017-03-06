/*
 * UDP Filtering Proxy
 * Copyright (C) 2017 Ivan Kuznetsov
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <event.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include "udp_filtering_proxy_config.h"
#include "incoming_from_server.h"
#include "log.h"
#include "simple_event_loop.h"

/**
 * Private:
 */

static pthread_t incoming_from_server_pthread; // second thread
static int world_socket_fd; // replying to clients through this socket
static struct event_base *event_base; // for second thread
static struct addrinfo *target_server_addrinfo;

void
cleanup() {
    event_base_free(event_base);
}

void
handle_packet_from_server(evutil_socket_t incoming_socket_fd,
                          short __attribute__ ((unused)) flags,
                          void *arg) {
    struct ClientData *client = arg;

    static char buf[READ_PACKET_BUFFER_SIZE] = {0};

    const ssize_t bytes_received = recv(incoming_socket_fd, buf,
                                        READ_PACKET_BUFFER_SIZE,
                                        0);
    if (bytes_received < 0) {
        log_warn("recv: server dead?");
        errno = 0;
        return;
    }


    const ssize_t bytes_sent = sendto(world_socket_fd,
                                      &buf, (size_t) bytes_received, 0,
                                      (struct sockaddr *) &client->client_address,
                                      client->client_address_len);

    if (bytes_sent < 0) {
        log_debug("recv: server dead?");
        errno = 0;
    } else {
        assert(bytes_sent == bytes_received);
    }
}

/**
 * Init incoming from server thread
 * @param incoming_world_socket_fd socket where clients connect
 * @param ip target
 * @param port
 */
void
start_incoming_from_server(const int incoming_world_socket_fd,
                           struct addrinfo *target_server) {
    world_socket_fd = incoming_world_socket_fd;
    target_server_addrinfo = target_server;

    pthread_attr_t pthread_params;

    if (pthread_attr_init(&pthread_params) != 0)
        fatal("pthread_attr_init");

    event_base = event_base_new();
    atexit(cleanup);

    const int success = pthread_create(&incoming_from_server_pthread,
                                       &pthread_params,
                                       simple_event_loop,
                                       event_base);
    if (success != 0)
        fatal("pthread_create");
}

/**
 * Create new socket for given ClientConnection
 * @param client
 * @return to_server socket FD
 */
int
add_new_client(struct ClientData *client) {
    assert(client->proxy_to_server_socket_fd == 0);

    int socketfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
    if (socketfd == -1)
        fatal("socket");

    const int rc = connect(socketfd, target_server_addrinfo->ai_addr,
                           target_server_addrinfo->ai_addrlen);
    if (rc == -1)
        fatal("connect");

    struct event *incoming_packet_event = event_new(event_base,
                                                    socketfd,
                                                    EV_READ | EV_PERSIST,
                                                    handle_packet_from_server,
                                                    client);
    if (event_add(incoming_packet_event, NULL) == -1)
        fatal("event_add");

    return socketfd;
}

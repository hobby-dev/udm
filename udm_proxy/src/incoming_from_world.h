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

#pragma once

#include <event.h>

int
create_world_socket(const uint16_t incoming_port);

struct event *
start_incoming_from_world(struct event_base *event_base,
                          struct addrinfo *target_server,
                          const int world_socket_fd);

void
handle_packet_from_world(evutil_socket_t incoming_socket_fd, short flags,
                         void *arg);

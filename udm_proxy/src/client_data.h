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

#include <arpa/inet.h>
#include <event.h>
#include <stdint.h>
#include <time.h>

struct ClientData {
    int proxy_to_server_socket_fd;
    uint64_t bits_till_ban;
    socklen_t client_address_len;
    struct sockaddr_in client_address;
    struct timespec last_change;
};

struct ClientData *
client_data_get_or_create(struct sockaddr_in *client_address, socklen_t i);

void
client_data_init();

void
client_data_cleanup();


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

#include <stdint.h>
#include <stdlib.h>

struct udp_filtering_proxy_settings {
    // listen ip
    uint16_t incoming_port;
    const char *target_ip;
    const char *target_port;
    // peer limits
    // shared memory ?
    int debug;
    size_t processors_count;
    size_t processors_stack_size;
};

int udp_filtering_proxy_start(const struct udp_filtering_proxy_settings *settings);


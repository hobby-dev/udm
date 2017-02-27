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

#include "udp_filtering_proxy.h"

int main() {
    struct udp_filtering_proxy_settings settings = {0};
    // TODO: parse args to fill settings
    settings.incoming_port = 12345;
    settings.target_ip = "localhost";
    settings.target_port = "31415";
    settings.debug = 1;
    settings.processors_count = 4; // Processor threads count
    settings.processors_stack_size = 1 << 15; // Processor thread stack 32 mb

    return udp_filtering_proxy_start(&settings);
}


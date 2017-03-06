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

#include <event.h>
#include <signal.h>

#include "log.h"
#include "simple_event_loop.h"
#include "incoming_common.h"
#include "client_data.h"

int keep_running = 1;

void
proxy_shutdown(evutil_socket_t __attribute__ ((unused)) incoming_socket_fd,
               short __attribute__ ((unused)) flags,
               void __attribute__ ((unused)) *arg) {
    keep_running = 0;
}

int
udp_filtering_proxy_start(const struct udp_filtering_proxy_settings *settings) {
    log_init(settings->debug);

    // 1. setup events and signals
    if (settings->debug)
        event_enable_debug_mode();

    struct event_base *event_base = event_base_new();

    struct event *sigint_event = event_new(event_base, SIGINT,
                                           EV_SIGNAL | EV_PERSIST,
                                           proxy_shutdown, NULL);
    if (event_add(sigint_event, NULL) == -1)
        fatal("unable to setup sigint signal");
    struct event *sigterm_event = event_new(event_base, SIGTERM,
                                            EV_SIGNAL | EV_PERSIST,
                                            proxy_shutdown, NULL);
    if (event_add(sigterm_event, NULL) == -1)
        fatal("unable to setup sigterm signal");

    client_data_init();

    incoming_init(settings, event_base);

    simple_event_loop(event_base);

    incoming_cleanup();

    client_data_cleanup();

    event_free(sigint_event);
    event_free(sigterm_event);
    event_base_free(event_base);

    return EXIT_SUCCESS;
}

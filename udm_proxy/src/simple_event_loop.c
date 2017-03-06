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

#include <event.h>
#include "simple_event_loop.h"

extern int keep_running;

void*
simple_event_loop(void* arg) {
    struct event_base *event_base = arg;
    struct timeval tv = {0};

    while (keep_running) {
        tv.tv_usec = 0;
        tv.tv_sec = 1;
        event_base_loopexit(event_base, &tv);
        event_base_dispatch(event_base);
    }

    return NULL;
}



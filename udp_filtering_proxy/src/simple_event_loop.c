#include <event.h>
#include "simple_event_loop.h"

extern int keep_running;

void*
simple_event_loop(void* arg) {
    struct event_base *event_base = arg;
    struct timeval tv;

    while (keep_running) {
        tv.tv_usec = 0;
        tv.tv_sec = 1;
        event_base_loopexit(event_base, &tv);
        event_base_dispatch(event_base);
    }

    return NULL;
}



#pragma once

#include <event.h>

#include "udp_filtering_proxy.h"

void
incoming_init(const struct udp_filtering_proxy_settings *settings,
              struct event_base *event_base);

void
incoming_cleanup();
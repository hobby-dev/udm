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
#include <arpa/inet.h>
#include <assert.h>

#include "incoming_from_world.h"
#include "log.h"
#include "udp_filtering_proxy_config.h"
#include "client_data.h"

static struct addrinfo *target_server_addrinfo;

int
create_world_socket(const uint16_t incoming_port) {
    const int incoming_socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK,
                                          IPPROTO_UDP);
    if (incoming_socket_fd == -1)
        fatal(NULL);

    // TODO: use getaddrinfo instead?
    struct sockaddr_in incoming_socket_addr;
    incoming_socket_addr.sin_family = AF_INET;
    incoming_socket_addr.sin_port = htons(incoming_port);
    incoming_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port:
    const int rc = bind(incoming_socket_fd, (struct sockaddr *)
                                &incoming_socket_addr,
                        sizeof(incoming_socket_addr));
    if (rc == -1)
        fatal(NULL);

    return incoming_socket_fd;
}

struct event *
start_incoming_from_world(struct event_base *event_base,
                          struct addrinfo *target_server,
                          const int world_socket_fd) {
    struct event *incoming_packet_event = event_new(event_base, world_socket_fd,
                                                    EV_READ | EV_PERSIST,
                                                    handle_packet_from_world,
                                                    NULL);
    target_server_addrinfo = target_server;
    if (event_add(incoming_packet_event, NULL) == -1)
        fatal("event_add");
    return incoming_packet_event;
}

uint64_t
diff_timespec(struct timespec *old, struct timespec *new) {
    assert(new->tv_sec >= 0 && new->tv_nsec >= 0
           && old->tv_sec >= 0 && old->tv_nsec >= 0);
    assert(old->tv_sec < new->tv_sec ||
           (old->tv_sec == new->tv_sec && old->tv_nsec < new->tv_nsec));

    uint64_t diff_seconds = (uint64_t) (new->tv_sec - old->tv_sec);
    uint64_t diff_nseconds = (uint64_t) (new->tv_nsec - old->tv_nsec);
    return diff_seconds * 1000000000 + diff_nseconds;
}

void
handle_packet_from_world(evutil_socket_t incoming_socket_fd,
                         short __attribute__ ((unused)) flags,
                         void __attribute__ ((unused)) *arg) {

    static char buf[READ_PACKET_BUFFER_SIZE] = {0};
    static struct sockaddr_in source_addr;
    socklen_t source_addr_len = sizeof(source_addr);

    const ssize_t bytes_received = recvfrom(incoming_socket_fd, buf,
                                            READ_PACKET_BUFFER_SIZE,
                                            0, (struct sockaddr *) &source_addr,
                                            &source_addr_len);

    if (bytes_received < 0) {
        log_debug("recvfrom");
        return;
    }

    // TODO: quick check if banned
    // TODO: export / import banned

    struct ClientData *client = client_data_get_or_create(
            &source_addr,
            source_addr_len);

    if (client->bits_till_ban == UINT64_MAX) {
        return;
    }

    struct timespec current_time;
    //TODO: update current time in main loop?
    const int rc = clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
    if (rc != 0)
        log_warn("clock_gettime");

    uint64_t passed_time = diff_timespec(&client->last_change, &current_time);
    client->last_change = current_time;

    if (passed_time < MIN_TIME_INTERVAL_BETWEEN_PACKETS_NANOSEC) {
        client->bits_till_ban = (client->bits_till_ban << 1) + 1;
        if (client->bits_till_ban == UINT64_MAX) {
            //TODO: add to fast banlist
            log_debug("ban set: %s\n", inet_ntoa(
                    client->client_address.sin_addr));
        }
    } else if (passed_time > RELAX_TIME_INTERVAL_BETWEEN_PACKETS_NANOSEC) {
        client->bits_till_ban = client->bits_till_ban >> 1;
    }

    const ssize_t bytes_sent = sendto(client->proxy_to_server_socket_fd,
                                      &buf, (size_t) bytes_received, 0,
                                      target_server_addrinfo->ai_addr,
                                      target_server_addrinfo->ai_addrlen);
    if (bytes_sent != bytes_received)
        log_warn("sendto");
}

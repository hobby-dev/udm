#include <netdb.h> // struct addrinfo
#include <stdlib.h>
#include <unistd.h> // close()

#include "log.h"
#include "incoming_common.h"
#include "incoming_from_world.h"
#include "incoming_from_server.h"

static int world_socket_fd;
static struct event *incoming_packet_event;
static struct addrinfo target_server_addrinfo;

/**
 * Private utility function
 * @param target_addrinfo
 * @param target_address
 * @param target_port
 * @return
 */
int
prepare_addrinfo(struct addrinfo *target_addrinfo, const char *target_address,
                 const char *target_port) {
    // Obtain address(es) matching TARGET_HOST/PORT

    struct addrinfo hints;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *result_addrinfo;
    const int exit_code = getaddrinfo(target_address, target_port, &hints,
                                      &result_addrinfo);
    if (exit_code != EXIT_SUCCESS)
        fatal(NULL);

    /*
    getaddrinfo() returns a list of address structures.
    Try each address until we successfully connect().
    If socket() (or connect()) fails, we (close the socket
    and) try the next address.
     */
    struct addrinfo *iterator;
    for (iterator = result_addrinfo;
         iterator != NULL; iterator = iterator->ai_next) {
        const int outgoing_socket_fd = socket(iterator->ai_family,
                                              iterator->ai_socktype,
                                              iterator->ai_protocol);
        if (outgoing_socket_fd == -1)
            continue; // try next address

        if (connect(outgoing_socket_fd, iterator->ai_addr,
                    iterator->ai_addrlen) == -1) {
            close(outgoing_socket_fd);
            continue; // try next address
        }

        // success!
        char host[NI_MAXHOST], service[NI_MAXSERV];
        const int getname_result = getnameinfo(iterator->ai_addr,
                                               iterator->ai_addrlen,
                                               host, NI_MAXHOST,
                                               service, NI_MAXSERV,
                                               NI_NUMERICHOST | NI_NUMERICSERV);
        if (getname_result == EXIT_SUCCESS) {
            log_debug("Successfully got target info for %s:%s\n", host,
                       service);
        } else {
            log_warn("%s", gai_strerror(getname_result));
        }

        *target_addrinfo = *iterator; // copy found addinfo
        freeaddrinfo(result_addrinfo);
        close(outgoing_socket_fd);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/**
 * Init incoming from World and from Server
 * @param settings incoming_port, target_ip, target_port
 * @param event_base incoming frow World events added here!
 */
void
incoming_init(const struct udp_filtering_proxy_settings *settings,
              struct event_base *event_base) {

    world_socket_fd = create_world_socket(settings->incoming_port);

    const int rc = prepare_addrinfo(&target_server_addrinfo,
                                    settings->target_ip,
                                    settings->target_port);
    if (rc != EXIT_SUCCESS)
        fatal("prepare_addrinfo: Can't get target address info");

    // init second thread
    start_incoming_from_server(world_socket_fd, &target_server_addrinfo);


    // setup incoming-from-world routine
    incoming_packet_event = start_incoming_from_world(event_base,
                                                      &target_server_addrinfo,
                                                      world_socket_fd);
}

/**
 * Free events and FDs
 */
void
incoming_cleanup() {
    event_free(incoming_packet_event);
    close(world_socket_fd);
}

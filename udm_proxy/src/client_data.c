#include <unistd.h>
#include <avl.h>
#include <malloc.h>
#include <string.h>

#include "client_data.h"
#include "incoming_from_server.h"
#include "log.h"

/**
 * AVL-tree:
 */
static avl_tree_t *clients_tree;

static int client_connection_compare(const void *arg1, const void *arg2) {
    const struct ClientData *client1 = arg1;
    const struct ClientData *client2 = arg2;
    int64_t sum1 = (client1->client_address.sin_addr.s_addr << 16) +
                   client1->client_address.sin_port;
    int64_t sum2 = (client2->client_address.sin_addr.s_addr << 16) +
                   client2->client_address.sin_port;
    return (int) (sum1 - sum2);
}

static void client_connection_free(void *arg1) {
    struct ClientData *client = arg1;
    close(client->proxy_to_server_socket_fd);
    free(client);
}


static struct ClientData *
client_connection_new(const struct sockaddr_in *client_address,
                      socklen_t client_address_len) {
    struct ClientData *new_client_connection = malloc(
            sizeof(struct ClientData));
    memset(new_client_connection, 0, sizeof(struct ClientData));
    new_client_connection->client_address = *client_address;
    new_client_connection->client_address_len = client_address_len;
    new_client_connection->proxy_to_server_socket_fd =
            add_new_client(new_client_connection);

    avl_node_t *insert_result = avl_insert(clients_tree,
                                           new_client_connection);
    if (insert_result == NULL)
        fatal("avl_insert");

    return new_client_connection;
}


/**
 * Get ClientConnection by it's address (or create new)
 * @param client_address
 * @param client_address_len
 * @return
 */
struct ClientData *
client_data_get_or_create(struct sockaddr_in *client_address,
                          socklen_t client_address_len) {
    struct ClientData test = {0};
    test.client_address = *client_address;
    // search for existing ClientData:
    avl_node_t *searchResult = avl_search(clients_tree, &test);
    if (searchResult == NULL) {
        return client_connection_new(client_address, client_address_len);
    }
    return searchResult->item;
}

/**
 * Must be called before client_connection_get_or_create()
 */
void
client_data_init() {
    clients_tree = avl_alloc_tree(&client_connection_compare,
                                  &client_connection_free);
}

/**
 * Free memory and FDs
 */
void
client_data_cleanup() {
    avl_free_tree(clients_tree);
}

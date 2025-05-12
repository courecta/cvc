#ifndef NETWORK_H
#define NETWORK_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_IP_LEN 16
#define PORT 8080

typedef struct {
    int sockfd;
    bool is_server;
    char remote_ip[MAX_IP_LEN];
    int remote_port;
    pthread_mutex_t lock;
} NetworkContext;

/**
 * @brief Initialize as a TCP server or client
 * @param is_server Run as server (true) or client (false)
 * @param remote_ip Server IP (client mode only)
 * @return Pointer to NetworkContext or NULL on failure
 */
NetworkContext* init_network(bool is_server, const char* remote_ip);

/**
 * @brief Send ASCII frame over TCP
 * @param ctx Network context
 * @param frame ASCII frame string
 * @return 0 on success, -1 on failure
 */
int send_frame(NetworkContext *ctx, const char *frame);

/**
 * @brief Receive ASCII frame over TCP
 * @param ctx Network context
 * @param buffer Output buffer for received frame
 * @param buffer_size Size of buffer
 * @return Number of bytes received, -1 on failure
 */
int receive_frame(NetworkContext *ctx, char *buffer, size_t buffer_size);

/**
 * @brief Clean up network resources
 * @param ctx Network context
 */
void free_network(NetworkContext *ctx);

#endif
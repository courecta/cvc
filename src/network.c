#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

NetworkContext* init_network(bool is_server, const char* remote_ip) {
    NetworkContext *ctx = calloc(1, sizeof(NetworkContext));
    if (!ctx) {
        perror("Memory allocation failed");
        return NULL;
    }

    if (pthread_mutex_init(&ctx->lock, NULL) != 0) {
        perror("Mutex initialization failed");
        free(ctx);
        return NULL;
    }

    ctx->is_server = is_server;
    if (!is_server && remote_ip) {
        strncpy(ctx->remote_ip, remote_ip, MAX_IP_LEN - 1);
        ctx->remote_port = PORT;
    }

    // Create socket
    ctx->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->sockfd < 0) {
        perror("Socket creation failed");
        goto fail;
    }

    if (is_server) {
        // Server setup
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(PORT);

        if (bind(ctx->sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Bind failed");
            goto fail;
        }

        if (listen(ctx->sockfd, 1) < 0) {
            perror("Listen failed");
            goto fail;
        }

        printf("Server listening on port %d\n", PORT);
        socklen_t addr_len = sizeof(addr);
        ctx->sockfd = accept(ctx->sockfd, (struct sockaddr*)&addr, &addr_len);
        if (ctx->sockfd < 0) {
            perror("Accept failed");
            goto fail;
        }
    } else {
        // Client setup
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, remote_ip, &server_addr.sin_addr) <= 0) {
            perror("Invalid address");
            goto fail;
        }

        if (connect(ctx->sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            goto fail;
        }
        printf("Connected to server at %s:%d\n", remote_ip, PORT);
    }

    pthread_mutex_init(&ctx->lock, NULL);
    return ctx;

fail:
    free_network(ctx);
    return NULL;
}

int send_frame(NetworkContext *ctx, const char *frame) {
    if (!ctx || !frame) return -1;
    pthread_mutex_lock(&ctx->lock);
    int len = strlen(frame);
    int bytes_sent = send(ctx->sockfd, frame, len, 0);
    if (bytes_sent < 0) {
        perror("Send failed");
    }
    pthread_mutex_unlock(&ctx->lock);
    return bytes_sent;
}

int receive_frame(NetworkContext *ctx, char *buffer, size_t buffer_size) {
    if (!ctx || !buffer) return -1;
    pthread_mutex_lock(&ctx->lock);
    int bytes_received = recv(ctx->sockfd, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate
    }
    pthread_mutex_unlock(&ctx->lock);
    return bytes_received;
}

void free_network(NetworkContext *ctx) {
    if (!ctx) return;
    close(ctx->sockfd);
    pthread_mutex_destroy(&ctx->lock);
    free(ctx);
}
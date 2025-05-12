#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Module headers
#include "video.h"
#include "ascii.h"
#include "ncurses_renderer.h"
#include "network.h"

// Constants
#define WIDTH 80   // Reduced from 120
#define HEIGHT 40  // Reduced from 70
#define FRAME_BUFFERS 2

// Global contexts
VideoCapture *vc = NULL;
NetworkContext *net_ctx = NULL;
WINDOW *win = NULL;

// Shared buffers
uint8_t gray_frame[WIDTH * HEIGHT];
char ascii_frame[WIDTH * HEIGHT * 2];

// Thread to capture video and convert to ASCII
void* video_thread_func(void* arg) {
    (void)arg;
    while (1) {
        if (read_frame(vc, gray_frame) == 0) {
            convert_to_ascii(gray_frame, WIDTH, HEIGHT, ascii_frame);
        }
        usleep(100000); // ~10 FPS
    }
    return NULL;
}

// Thread to send ASCII frames over TCP
void* send_thread_func(void* arg) {
    (void)arg;
    while (1) {
        if (ascii_frame[0] != '\0') {
            send_frame(net_ctx, ascii_frame);
        }
        usleep(100000); // Match FPS
    }
    return NULL;
}

// Thread to receive and render remote ASCII frames
void* receive_thread_func(void* arg) {
    (void)arg;
    char recv_buffer[WIDTH * HEIGHT * 2];
    while (1) {
        int bytes = receive_frame(net_ctx, recv_buffer, sizeof(recv_buffer));
        if (bytes > 0) {
            render_ascii_frame(win, recv_buffer);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Initialize video capture
    vc = init_video_capture("/dev/video0", WIDTH, HEIGHT);
    if (!vc) {
        fprintf(stderr, "Failed to initialize video capture\n");
        goto cleanup;
    }

    // Initialize ncurses renderer
    win = init_ncurses_renderer(WIDTH, HEIGHT);
    if (!win) {
        fprintf(stderr, "Failed to initialize ncurses\n");
        goto cleanup;
    }

    // Initialize network context (server or client)
    bool is_server = (argc > 1 && strcmp(argv[1], "--server") == 0);
    const char *remote_ip = (argc > 2) ? argv[2] : "127.0.0.1";
    net_ctx = init_network(is_server, remote_ip);
    if (!net_ctx) {
        fprintf(stderr, "Failed to initialize network\n");
        goto cleanup;
    }

    // Create threads
    pthread_t video_thread, send_thread, receive_thread;
    pthread_create(&video_thread, NULL, video_thread_func, NULL);
    pthread_create(&send_thread, NULL, send_thread_func, NULL);
    pthread_create(&receive_thread, NULL, receive_thread_func, NULL);

    // Main loop: Render local frames (optional)
    while (1) {
        if (ascii_frame[0] != '\0') {
            render_ascii_frame(win, ascii_frame);
        }
        usleep(100000); // Match FPS
    }

cleanup:{
    shutdown_ncurses_renderer();
    free_video_capture(vc);
    free_network(net_ctx);
}
    if (pthread_join(video_thread, NULL) != 0) {
        perror("Failed to join video thread");
    }
    if (pthread_join(send_thread, NULL) != 0) {
        perror("Failed to join send thread");
    }
    if (pthread_join(receive_thread, NULL) != 0) {
        perror("Failed to join receive thread");
    }

    return 0;
}
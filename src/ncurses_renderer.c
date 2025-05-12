#include "ncurses_renderer.h"
#include <stdio.h>
#include <stdlib.h>

WINDOW* init_ncurses_renderer(int width, int height) {
    // Initialize ncurses
    WINDOW *win = initscr();
    if (!win) {
        fprintf(stderr, "Failed to initialize ncurses\n");
        return NULL;
    }

    // Configure terminal
    noecho();            // Don't echo input
    cbreak();            // Disable line buffering
    curs_set(0);         // Hide cursor
    keypad(win, TRUE);   // Enable arrow keys

    // Resize terminal window (if needed)
    if (width > 0 && height > 0) {
        resize_term(height, width);
    }

    // Clear screen
    wclear(win);
    wrefresh(win);

    return win;
}

void render_ascii_frame(WINDOW *win, const char *ascii_buffer) {
    if (!win || !ascii_buffer) return;

    wclear(win); // Clear previous frame

    // Print ASCII frame line-by-line
    int y = 0, x = 0;
    const char *ptr = ascii_buffer;
    while (*ptr) {
        if (*ptr == '\n') {
            y++;
            x = 0;
        } else {
            mvwprintw(win, y, x, "%c", *ptr);
            x++;
        }
        ptr++;
    }

    wrefresh(win); // Update terminal display
}

void shutdown_ncurses_renderer() {
    endwin(); // Restore terminal settings
}
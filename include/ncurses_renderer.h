#ifndef NCURSES_RENDERER_H
#define NCURSES_RENDERER_H

#include <curses.h> // ncurses header

/**
 * @brief Initialize ncurses renderer
 * @param width Terminal width (in characters)
 * @param height Terminal height (in lines)
 * @return Pointer to WINDOW object or NULL on failure
 */
WINDOW* init_ncurses_renderer(int width, int height);

/**
 * @brief Render an ASCII frame in the terminal
 * @param win ncurses window
 * @param ascii_buffer ASCII art string (with newlines)
 */
void render_ascii_frame(WINDOW *win, const char *ascii_buffer);

/**
 * @brief Clean up ncurses resources
 */
void shutdown_ncurses_renderer();

#endif
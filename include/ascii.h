#ifndef ASCII_H
#define ASCII_H

#include <stdint.h>

/**
 * @brief Convert a grayscale frame to ASCII art
 * @param gray_buffer Input grayscale pixel buffer (size: width * height)
 * @param width Frame width
 * @param height Frame height
 * @param ascii_buffer Output buffer for ASCII characters (size: width * height + height for newlines)
 * @return 0 on success, -1 on failure
 */
int convert_to_ascii(const uint8_t *gray_buffer, int width, int height, char *ascii_buffer);

#endif
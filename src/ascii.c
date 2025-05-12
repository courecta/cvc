#include "ascii.h"
#include <string.h>
#include <stdio.h>

// ASCII character set from darkest to lightest (grayscale mapping)
// Reference: https://www.jonathan-petitcolas.com/2017/12/28/convert-image-to-ascii-art.html  [[4]]
static const char ASCII_CHARS[] = "@#S%?*+;:,. ";

int convert_to_ascii(const uint8_t *gray_buffer, int width, int height, char *ascii_buffer) {
    if (!gray_buffer || !ascii_buffer || width <= 0 || height <= 0) {
        return -1;
    }

    const int char_count = strlen(ASCII_CHARS);
    int buffer_idx = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t gray = gray_buffer[y * width + x];
            // Map grayscale value to ASCII character
            char ascii_char = ASCII_CHARS[(gray * (char_count - 1)) / 255];
            ascii_buffer[buffer_idx++] = ascii_char;
        }
        ascii_buffer[buffer_idx++] = '\n'; // Newline after each row
    }

    ascii_buffer[buffer_idx] = '\0'; // Null-terminate the string
    return 0;
}
#ifndef FONT_H
#define FONT_H

#include <stdint.h>

static const uint8_t Empty[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

static const uint8_t A[] = {
    0b00111100,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00000000,
};

static const uint8_t Z[] = {
    0b01111110,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01100000,
    0b01111110,
    0b00000000,
};

#endif

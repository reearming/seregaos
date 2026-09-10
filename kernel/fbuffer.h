#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include "limine.h"

void framebuffer_init(struct limine_framebuffer *fb);
void print_char(char c, uint32_t color);
void print(const char *str);

void put_pixel(uint32_t x, uint32_t y, uint32_t color,
               struct limine_framebuffer fb);

void put_char(uint32_t x, uint32_t y, uint32_t color,
              struct limine_framebuffer fb, const uint8_t letter[]);

#endif

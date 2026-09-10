#include "fbuffer.h"
#include <stdint.h>
#include "font.h"

static struct limine_framebuffer *framebuffer;
static uint32_t cursor_x;
static uint32_t cursor_y;

void framebuffer_init(struct limine_framebuffer *fb)
{
    framebuffer = fb;
    cursor_x = 0;
    cursor_y = 0;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color,
               struct limine_framebuffer fb)
{
    uint32_t *pixpoint = (uint32_t *)fb.address;
    pixpoint[x + (y * (fb.pitch / 4))] = color;
}

void put_char(uint32_t x, uint32_t y, uint32_t color,
              struct limine_framebuffer fb, const uint8_t letter[])
{
    uint32_t *pixpoint = (uint32_t *)fb.address;
    
    x += 7;
    for (uint32_t i = 0; i < 8; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            if (letter[i] >> j & 1)
            {
                pixpoint[x - j + ((y + i) * fb.pitch / 4)] = color;
            }
        }
    }
}

void print_char(const char c, uint32_t color)
{
    put_char(cursor_x, cursor_y, color, *framebuffer, font[c]);
    cursor_x += 8;

    if (cursor_x + 8 > framebuffer->width) {
        cursor_x = 0;
        cursor_y += 8;
    }
}

void print(const char *string)
{
    if (!string) 
        return;


    for (uint32_t i = 0; string[i] != '\0'; i++)
    {
       print_char(string[i], 0xFFFFFF);
    }
}

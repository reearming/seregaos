#include "fbuffer.h"
#include <stdarg.h>
#include <stdint.h>
#include "font.h"
#include "colors.h"
#include "mathf.h"

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
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 10;
        return;
    }

    put_char(cursor_x, cursor_y, color, *framebuffer, font[c]);
    cursor_x += font_advance[c];

    if (cursor_x + 8 > framebuffer->width) {
        cursor_x = 0;
        cursor_y += 10;
    }
}

void print(const char *string)
{
    uint32_t color = 0xFFFFFF;
    
    if (!string) 
        return;


    for (uint32_t i = 0; string[i] != '\0'; i++)
    {
        if (string[i] == '%') {
            switch (string[i+1]) {
                case 'R':
                    color = red;
                    i++;
                    continue;
                case 'W':
                    color = white;
                    i++;
                    continue;
                case 'G':
                    color = green;
                    i++;
                    continue;
                case 'B':
                    color = blue;
                    i++;
                    continue;
        
            }
        }
        print_char(string[i], color);
    }
}

void print_hex(uint64_t value, uint32_t color) {
    print_char('0', color);
    print_char('x', color);
    
    if (value == 0) {
        print_char('0', color);
        return;
    }

    uint8_t leading_zeros = 1;

    for (int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0x0F;

        if (nibble != 0)
            leading_zeros = 0;

        if (leading_zeros == 0) print_char(hex_digits[nibble], color);
    }
}

void print_dec(int64_t value, uint32_t color) {
    if (value < 0) {
        print_char('-', color);
        value = -value;
    }
    if (value == 0) {
        print_char('0', color);
        return;
    }

    int start_power = 0;
    while (pow(10, start_power + 1) <= value && start_power < 18) {
        start_power++;
    }

    for (int i = start_power; i >= 0; i--) {
        uint8_t digit = (value / pow(10, i)) % 10 + '0';

        print_char(digit, color);
    }
}

void printf(char *format, ...) {
    va_list args;
    
    uint32_t color = 0xFFFFFF;
    
    if (!format) 
        return;
    
    va_start(args, format);

    for (uint32_t i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'R':
                    color = red;
                    continue;
                case 'W':
                    color = white;
                    continue;
                case 'G':
                    color = green;
                    continue;
                case 'B':
                    color = blue;
                    continue;
                case 'd':
                    print_dec(va_arg(args, int), color);
                    continue;
                case 'c':
                    print_char(va_arg(args, int), color);
                    continue;
                case 's':
                    print(va_arg(args, char *));
                    continue;
                case 'x':
                    print_hex(va_arg(args, unsigned int), color);
                    continue;
                case '%':
                    print_char('%', color);
                    continue;
                default:
                    print_char('%', color);
                    break;
            }
        }
        print_char(format[i], color);
    }
}

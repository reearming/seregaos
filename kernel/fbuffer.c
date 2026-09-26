#include <stdarg.h>
#include <stdint.h>
#include "font.h"
#include "colors.h"
#include "mathf.h"
#include "limine.h"
#include "stddef.h"

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

static volatile struct limine_framebuffer_request framebuffer_request
    __attribute__((section(".limine_requests"), used)) =
{
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

static struct limine_framebuffer *framebuffer;
static uint32_t cursor_x;
static uint32_t cursor_y;


void framebuffer_init()
{
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        for (;;) asm volatile("hlt");
    }
    framebuffer = framebuffer_request.response->framebuffers[0];
    cursor_x = 0;
    cursor_y = 0;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color, struct limine_framebuffer *fb)
{
    uint32_t *pixpoint = (uint32_t *)framebuffer->address;
    pixpoint[x + (y * (fb->pitch / 4))] = color;
}

void put_char(uint32_t x, uint32_t y, uint32_t color, struct limine_framebuffer *fb, const uint8_t letter[])
{
    x += 7;
    for (uint32_t i = 0; i < 8; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            if (letter[i] >> j & 1)
            {
                put_pixel(x - j, y + i, color, fb);
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

    put_char(cursor_x, cursor_y, color, framebuffer, font[c]);
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

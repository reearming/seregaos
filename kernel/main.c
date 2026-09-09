#include <stddef.h>
#include <stdint.h>
#include "limine.h"
#include "font.h"

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



void put_pixel(uint32_t x, uint32_t y, uint32_t color, struct limine_framebuffer fb )
{
    uint32_t *pixpoint = (uint32_t *)fb.address;
    pixpoint[x + (y * (fb.pitch / 4))] = color;
}

void put_char(uint32_t x, uint32_t y, uint32_t color, struct limine_framebuffer fb, const uint8_t letter[])
{
    uint32_t *pixpoint = (uint32_t *)fb.address;

    for (uint32_t i = 0; i < 8; i++)
    { 
        for (uint32_t j = 8; j > 0; j--)
        {
            if (letter[i]>>j & 1)
            {
                pixpoint[x - j + ((y + i) * fb.pitch / 4)] = color;
            }
        }
     }
}

void kmain(void) {
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        for (;;) asm("hlt");
    }
    
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    /*for (size_t i = 0; i < framebuffer->width; i++)
    {
        for (size_t j = 0; j < framebuffer->height; j++) {
            put_pixel(i, j, 0xFFFFFF, *framebuffer); 
        }
    }*/

    put_char(50, 65, 0xFFFFFF, *framebuffer, Z);
    put_char(60, 65, 0xFFFFFF, *framebuffer, Z);

    for (;;) { }
}

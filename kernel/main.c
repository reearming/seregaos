#include <stddef.h>
#include <stdint.h>
#include "limine.h"
#include "font.h"
#include "fbuffer.h"

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

    framebuffer_init(framebuffer);

    print("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG\0");

    for (;;) { }
}

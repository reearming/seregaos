#include <stddef.h>
#include <stdint.h>
#include "limine.h"
#include "fbuffer.h"
#include "idt.h"
#include "colors.h"

extern void set_gdt();

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
    set_gdt();
    idt_init();

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        for (;;) asm volatile("hlt");
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    framebuffer_init(framebuffer);

    print("%RRED, %GGREEN, %BBLUE, %WWHITE\n");

    printf("DEC: %d, HEX: %x, CHAR: %c, STRING: %s\n", 0xFF, 0xFF, 'x', "%GWA%BTER%WME%RLON");

    printf("HERE WILL BE PAGE FAULT: %s", 65);

//    asm("int $0x0A");

    for (;;) asm volatile("hlt");
}

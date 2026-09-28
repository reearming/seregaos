#include <stddef.h>
#include <stdint.h>
#include "fbuffer.h"
#include "idt.h"
#include "apic.h"
#include "limine.h"
#include "IO.h"

extern void set_gdt();

/*__attribute((used, section(".limine_requests")))
static volatile struct limine_paging_mode_request paging_request = {
    .id = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL,
};*/

void kmain() { 
    framebuffer_init();
     
    set_gdt();
    idt_init();
    
    asm("cli");
    disable_pic();
    init_x2apic();
    //init_apic_acpi();
    ioapic_keyboard(0x33);
    asm("sti");

    //print("%RRED, %GGREEN, %BBLUE, %WWHITE\n");
    //printf("DEC: %d, HEX: %x, CHAR: %c, STRING: %s\n", 0xFF, 0xFF, 'x', "%GWA%BTER%WME%RLON");
    //printf("HERE WILL BE PAGE FAULT: %s", 65);
    print("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG\n\n");
    print("the quick brown fox jumps over the lazy dog");

    for (;;) asm volatile("hlt");
}

#include "fbuffer.h"
#include <stdint.h>
#include "IO.h"

#define PIC1_COMMAND 0x20
#define PIC_EOI 0x20 

void printkey() {
    uint8_t scancode = inb(0x60);

    print_char(scancode, 0xFFFFFF);

    outb(PIC1_COMMAND, PIC_EOI);
}

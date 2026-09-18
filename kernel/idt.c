#include <stdint.h>
#include "keyboard.h"

struct InterruptDescriptor64 {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
};

struct InterruptDescriptor64 idt[256] __attribute__((aligned(16)));

struct __attribute__((packed)) idtr {
    uint16_t limit; //size
    uint64_t base; // idt pointer
};

void idtgateset(void *addr, uint16_t selector, uint8_t ist, uint8_t type_attributes, uint8_t vector) {
    uint64_t handler = (uint64_t)addr;
    
    idt[vector].offset_1 = (uint16_t)(handler & 0xFFFF);
    idt[vector].selector = selector;
    idt[vector].ist = ist;
    idt[vector].type_attributes = type_attributes;
    idt[vector].offset_2 = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[vector].offset_3 = (uint64_t)((handler >> 32) & 0xFFFFFFFF);
    idt[vector].zero = 0;
}

void idtinit() {
    struct idtr idtr;
    idtr.limit = (uint16_t)sizeof(struct InterruptDescriptor64);
    idtr.base = (uint64_t)&idt;

    idtgateset(printkey, 0x08, 0, 0x8E, 33);
    idtgateset(printkey, 0x08, 0, 0x8E, 0);

    __asm__ volatile("lidt %0" : : "m"(idtr));
    __asm__ volatile("sti");
}

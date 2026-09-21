#include <stdint.h>
#include <sys/cdefs.h>
#include "handlers.h"
#include "IO.h"

struct interrupt_descriptor64 {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
};

struct interrupt_descriptor64 idt[256] __attribute__((aligned(16)));

struct __attribute__((packed)) idtr {
    uint16_t limit; //size
    uint64_t base; // idt pointer
};

void idt_gate_set(void *addr, uint16_t selector, uint8_t ist, uint8_t type_attributes, uint8_t vector) {
    uint64_t handler = (uint64_t)addr;
    
    idt[vector].offset_1 = (uint16_t)(handler & 0xFFFF);
    idt[vector].selector = selector;
    idt[vector].ist = ist;
    idt[vector].type_attributes = type_attributes;
    idt[vector].offset_2 = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[vector].offset_3 = (uint64_t)((handler >> 32) & 0xFFFFFFFF);
    idt[vector].zero = 0;
}

void idt_init() {
    struct idtr idtr;
    idtr.limit = (uint16_t)sizeof(struct interrupt_descriptor64) * 32 - 1;
    idtr.base = (uint64_t)&idt;

    for (int i = 0; i < 32; i++)
    {
        if (i == 3)
            idt_gate_set((void *)isr[i], 0x08, 0, 0xFF, i);
        else
            idt_gate_set((void *)isr[i], 0x08, 0, 0x8E, i);
    }

    asm volatile("lidt %0" : : "m"(idtr));
    asm volatile("sti");
}

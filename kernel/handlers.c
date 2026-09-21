#include "fbuffer.h"
#include <stdint.h>
#include "IO.h"

struct interrupt_regs {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rsi, rdi, rdx, rcx, rbx, rax;

    uint64_t num; //vector
    uint64_t errcode;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

void print_key() {
    //uint8_t scancode = inb(0x60);

    print_char('A', 0xFFFFFF);

    //outb(PIC1_COMMAND, PIC_EOI);
}

void excstop(const char *out) {
    print(out);

    for (;;) asm("cli; hlt");
}

void interrupt_dispatcher(struct interrupt_regs *regs) {
   
    switch (regs->num) {
        case 0: //divide error
            excstop("DIVIDE ERROR, STOPPING...");
            break;

        case 3: //breakpoint (int3)
            excstop("BREAKPOINT");
            break;

        case 6: //invalid opcode
            excstop("INVALID OPCODE, STOPPING...");
            break;
        
        case 14: //page fault
            excstop("PAGE FAULT: " + regs->errcode);
            break;
        
        default:
            print("UNHANDLED INTERRUPT");
            break;
    }
}

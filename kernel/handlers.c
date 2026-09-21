#include "fbuffer.h"
#include <stdint.h>
#include "IO.h"
#include "colors.h"

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

void stop() {
    asm("cli; hlt");
}

void print_key() {
    //uint8_t scancode = inb(0x60);

    print_char('A', 0xFFFFFF);

    //outb(PIC1_COMMAND, PIC_EOI);
}

void excstop(const char *out) {
    print(out);
    
    stop();
}

void interrupt_dispatcher(struct interrupt_regs *regs) {
   
    switch (regs->num) {
        case 0: //divide error
            excstop("%RDIVIDE ERROR!");
            break;

        case 1: //debug exception
            excstop("DEBUG EXCEPTION");
            break;

        case 2: //nonmaskable external interrupt
            excstop("NMI");
            break;

        case 3: //breakpoint (int3)
            excstop("BREAKPOINT");
            break;

        case 4: //overflow
            excstop("OVERFLOW");
            break;

        case 5: //BOUND range exceeded 
            excstop("%BOUND RANGE EXCEEDED!");
            break;

        case 6: //invalid opcode
            excstop("%RINVALID OPCODE!");
            break;

        case 7: //device not available
            excstop("%RDEVICE NOT AVAILABLE!");
            break;

        case 8: //double fault
            
            break;

        case 9: break; //segment overrun
                       
        case 10: //invalid tss
            print("%RINVALID TSS: ");
            print_hex(regs->errcode, red);
            stop();
            break;

        case 11: //segment not present
            print("%RSEGMENT NOT PRESENT: ");
            print_hex(regs->errcode, red);
            stop();
            break;

        case 12: //stack-segment fault
            print("%RSTACK-SEGMENT FAULT: ");
            print_hex(regs->errcode, red);
            stop();
            break;

        case 13: //general protection
            print("%RGENERAL PROTECTION: ");
            print_hex(regs->errcode, red);
            stop();
            break;
        
        case 14: //page fault
            print("%RPAGE FAULT: ");
            print_hex(regs->errcode, red);
            stop();
            break;
        
        default:
            print("UNHANDLED INTERRUPT");
            break;
    }
}

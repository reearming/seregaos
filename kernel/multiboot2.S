/*  boot.S - bootstrap the kernel */
/*  Copyright (C) 1999, 2001, 2010  Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define ASM_FILE        1
#include "multiboot2.h"

/*  C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/*  The size of our stack (16KB). */
#define STACK_SIZE                      0x4000

/*  The flags for the Multiboot header. */
#ifdef __ELF__
# define AOUT_KLUDGE 0
#else
# define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif
        .text

.align  8
        /*  Multiboot header. */
multiboot_header:
        /*  magic */
        .long   MULTIBOOT2_HEADER_MAGIC
        /*  ISA: i386 */
        .long  0 
        /*  Header length. */
        .long   multiboot_header_end - multiboot_header
        /*  checksum */
        .long   -(MULTIBOOT2_HEADER_MAGIC + 0 + (multiboot_header_end - multiboot_header))
.align 8
EFI_amd64_tag_start:
        .short 9
        .short 1 
        .long EFI_amd64_tag_end - EFI_amd64_tag_start
        .long EFI_start
EFI_amd64_tag_end:
.align 8
#ifndef __ELF__
address_tag_start:      
        .short MULTIBOOT_HEADER_TAG_ADDRESS
        .short MULTIBOOT_HEADER_TAG_OPTIONAL
        .long address_tag_end - address_tag_start
        /*  header_addr */
        .long   multiboot_header
        /*  load_addr */
        .long   _start
        /*  load_end_addr */
        .long   _edata
        /*  bss_end_addr */
        .long   _end
address_tag_end:
.align 8
entry_address_tag_start:        
        .short MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
        .short MULTIBOOT_HEADER_TAG_OPTIONAL
        .long entry_address_tag_end - entry_address_tag_start
        /*  entry_addr */
        .long _start
entry_address_tag_end:
.align 8
#endif /*  __ELF__ */
framebuffer_tag_start:  
        .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
        .short MULTIBOOT_HEADER_TAG_OPTIONAL
        .long framebuffer_tag_end - framebuffer_tag_start
        .long 1024
        .long 768
        .long 32
framebuffer_tag_end:
.align 8
        .short MULTIBOOT_HEADER_TAG_END
        .short 0
        .long 8
multiboot_header_end:

.code64
.align 16
.globl EFI_start
EFI_start:
    //movq stack_end, %rsp
    movl %eax, %edi
    movl %ebx, %esi
    .extern kmain
    call    kmain
    longhalt:
        hlt
        jmp longhalt

.code32
.align 8
.globl _start
_start:
        cli

        movl    %ebx, %esi
        movl    %eax, %edx


        lgdt gdtr
        ljmp $0x08, $flush

        flush:
        movw $0x10, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %ss
        movw %ax, %fs
        movw %ax, %gs


        //movw $0x0A4B, %ds:0xB8DC2

        //jmp halt

        /*  Initialize the stack pointer. */
        movl    stack_end, %esp

        /*  Reset EFLAGS. */
        pushl   $0
        popf
 
        /*  Push the pointer to the Multiboot information structure. */
        pushl   %ebx

        /*  Push the magic value. */
        pushl   %eax


        .extern entry
        jmp entry

        halt:
            hlt
            jmp     halt


.bss
stack:
    .skip 16384
stack_end:

.data
.align 4
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
gdt_end:

gdtr:
    .word gdt_end - gdt_start - 1
    .long gdt_start



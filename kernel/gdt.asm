bits 64

global gdtr
global setGdt
global gdt_start

section .data
align 16
gdt_start:
    ; 1. Null (0x00)
    dq 0x0000000000000000 

    ; 2. Kernel Code (0x08)
    dq 0x00AF9A000000FFFF

    ; 3. Kernel Data (0x10)
    dq 0x00AF92000000FFFF

    ; 4. User Data (0x18)
    dq 0x0000F20000000000

    ; 5. User Code (0x20)
    dq 0x0020FA0000000000
gdt_end:

align 4
gdtr: DW gdt_end - gdt_start - 1 ; For limit storage
     DQ gdt_start ; For base storage

section .text

setGdt:
    lgdt  [gdtr]

    push 0x08                  
    lea rax, [.reload_cs]       
    push rax
    retfq                      

.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret 

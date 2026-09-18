bits 64

global commonstub
extern interruptdis

section .text

%macro PUSH_ALL 0
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r15
%endmacro

%macro POP_ALL 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

commonstub:
    PUSH_ALL
    
    mov rdi, rsp
    call interruptdis

    POP_ALL
    add rsp, 16
    iretq

%macro ISR_NOERRORCODE 1
global isr_%1
isr_%1:
    push qword r10
    push qword %1
    jmp commonstub
%endmacro


ISR_NOERRORCODE 0
ISR_NOERRORCODE 33

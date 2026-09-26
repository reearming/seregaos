bits 32
section .data
; it may be preferrable to put this in a separate file to be included,
; along with any other EFLAGS bits you may want to use
EFLAGS_ID equ 1 << 21           ; if this bit can be flipped, the CPUID
                                ; instruction is available

; Checks if CPUID is supported by attempting to flip the ID bit (bit 21) in
; the EFLAGS register. If we can flip it, CPUID is available.
; returns eax = 1 if there is cpuid support; 0 otherwise
global check_cpuid
section .text
check_cpuid:
    pushfd
    pop eax

    ; The original value should be saved for comparison and restoration later
    mov ecx, eax
    xor eax, EFLAGS_ID

    ; storing the eflags and then retrieving it again will show whether or not
    ; the bit could successfully be flipped
    push eax                    ; save to eflags
    popfd
    pushfd                      ; restore from eflags
    pop eax

    ; Restore EFLAGS to its original value
    push ecx
    popfd

    ; if the bit in eax was successfully flipped (eax != ecx), CPUID is supported.
    xor eax, ecx
    jnz .supported
    .notSupported:
        mov ax, 0
        ret
    .supported:
        mov ax, 1
        ret

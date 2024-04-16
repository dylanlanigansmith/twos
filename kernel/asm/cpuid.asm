[bits 64]

global has_cpuid:function
;returns 0 if no cpuid

;this is straight from OSDev https://wiki.osdev.org/CPUID
; i just changed to 64 bit

CPU_ID_BITMASK equ 0x00200000 

has_cpuid: ;we arent ever targeting systems where this isnt available but lets use this helper func from wiki for prosperity's sake 
    pushfq                               ;Save EFLAGS
    pushfq                               ;Store EFLAGS
    xor qword [rsp], CPU_ID_BITMASK         ;Invert the ID bit in stored EFLAGS
    popfq                                ;Load stored EFLAGS (with ID bit inverted)
    pushfq                               ;Store EFLAGS again (ID bit may or may not be inverted)
    pop rax                              ;eax = modified EFLAGS (ID bit may or may not be inverted)
    xor rax,[rsp]                        ;eax = whichever bits were changed
    popfq                                ;Restore original EFLAGS
    and rax, CPU_ID_BITMASK                 ;eax = zero if ID bit can't be changed, else non-zero
    ret
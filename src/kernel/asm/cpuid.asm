[bits 64]

section .text
global has_cpuid:function
global cpuid_proc_feat_info





CPU_ID_BITMASK equ 0x00200000 
;this is straight from OSDev https://wiki.osdev.org/CPUID ; i just changed to 64 bit
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
    ret ;returns 0 if no cpuid



cpuid_proc_feat_info:
    push rbp
    mov rbp, rsp
    mov eax, 1
    cpuid
    mov dword [rdi], eax
    mov dword [rdi + 4], edx
    mov dword [rdi + 8], ecx
    mov dword [rdi + 12], ebx
    mov rsp, rbp
    pop rbp
    ret
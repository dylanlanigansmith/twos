[bits 64]

section .text

global get_cr3

get_cr3:
    mov rax, cr3 
    ret


global get_cr2
get_cr2:
    mov rax, cr2 
    ret

global swap_cr3:function

swap_cr3: ;rdi = new cr3, returns old in rax
    mov rax, cr3
    mov cr3, rdi
    ret
    
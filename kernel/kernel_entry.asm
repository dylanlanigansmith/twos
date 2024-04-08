[bits 32]
; this is linked always at the start of our kernel code block so it can be called via the offset by the boot sector

stack_end:
    times 8192 db 0 ;this is so fucking greasy and shouldnt work but it does and that scares me
stack_start:

[extern main]
mov ebp, stack_start
mov esp, ebp
call main 
;if kernel comes back
cli
hlt
jmp $



%include 'kernel/asm/idt.asm'
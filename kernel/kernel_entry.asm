[bits 32]
; this is linked always at the start of our kernel code block so it can be called via the offset by the boot sector



[extern main]
call main 
jmp $


%include 'kernel/asm/idt.asm'
; GDT
gdt_start:

gdt_null:
    dd 0x0 ; dd == define double word (4 bytes)
    dd 0x0
gdt_code: ; code segment descriptor
    dw 0xfffff
    dw 0x0
    db 0x0
    db 10011010b ; type flags
    db 11001111b ; limit flags
    db 0x0 ; base
gdt_data:
    dw 0xfffff ;okay bro u should really rewrite this in c
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end: ;labels start & end used so assembler can calculate size

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ;size of gdt (always one less)
    dd gdt_start ; start addr of gdt

;from textbook: pg 40
; Define some handy constants for the GDT segment descriptor offsets , which
; are what segment registers must contain when in protected mode. For example ,
; when we set DS = 0 x10 in PM , the CPU knows that we mean it to use the
; segment described at offset 0 x10 ( i.e. 16 bytes ) in our GDT , which in our
; case is the DATA segment (0 x0 -> NULL ; 0x08 -> CODE ; 0 x10 -> DATA )
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start



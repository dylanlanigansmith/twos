[bits 32]

%macro isr_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret 
%endmacro
; if writing for 64-bit, use iretq instead
%macro isr_no_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

extern exception_handler
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i 
%assign i i+1 
%endrep


VIDEO_MEMORY equ 0xb8000
RED_ON_BLACK equ 0x0C ;https://wiki.osdev.org/Text_UI colros

PANIC_MSG   db 'Kernel Panic',0

global panic
panic:
    mov ebx, PANIC_MSG
; copied from boot/pm bc whatever
    mov edx, VIDEO_MEMORY
print_panic_loop:
    mov al, [ebx] ; character in low
    mov ah, RED_ON_BLACK  ; attrb in high

    cmp al, 0
    je panic_halt

    mov [edx], ax ;write to char in vid mem
    add ebx, 1 ;move to next char in string
    add edx, 2 ; move to next char cell
    jmp print_panic_loop

panic_halt:
    jmp $
    hlt ;dunno
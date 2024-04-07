[bits 32]
VIDEO_MEMORY equ 0xb8000
RED_ON_BLACK equ 0x0C 
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
[bits 16]

clear_screen_real:
    pusha

    mov ax, 0x0700 ; func 07, al-0 = scroll whole window
    mov bh, 0x07 ; white on black
    mov cx, 0x0000 ; row 0 col 0
    mov dx, 0x184f ; row 24 col 79
    int 0x10 ; call vid interupt
    popa 
    ret

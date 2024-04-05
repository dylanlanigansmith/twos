[bits 32]
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f




print_str_pm:
    pusha 
    mov edx, VIDEO_MEMORY
print_str_pm_loop:
    mov al, [ebx] ; character in low
    mov ah, WHITE_ON_BLACK  ; attrb in high

    cmp al, 0
    je print_str_pm_done

    mov [edx], ax ;write to char in vid mem
    add ebx, 1 ;move to next char in string
    add edx, 2 ; move to next char cell
    jmp print_str_pm_loop

print_str_pm_done:
    popa 
    ret
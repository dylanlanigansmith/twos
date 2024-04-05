[bits 16]
print_str:
    pusha ;push all reg vals to stack
    mov ah, 0x0e ; int 10/ ah = 0eh -> scrolling teletype BIOS routine
    mov cx, 0
print_str_char:
    mov al, [bx]
    cmp al, 0 ;is this the end of string ?
    je end_string 
    int 0x10 ;call interupt to print al
    add bx, 1 ;add 1 to string offset to get next char
    jmp print_str_char 
end_string:
    popa ;return regs to valuebefore call
    ret

print_char: ;takes char in al and prints it
    pusha
    mov ah, 0x0e
    int 0x10
    popa
    ret
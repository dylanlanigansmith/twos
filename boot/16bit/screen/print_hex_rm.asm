[bits 16]


print_hex:
    mov bx, HEX_OUT
    mov cx, dx
    add bx, 2
    ;manipualte HEX_OUT string
    shr dx, 12
    and dx, 0xf 

    call apply_hex_char
    mov dx, cx
    add bx, 1 
    shr dx, 8
    and dx, 0xf 
    call apply_hex_char

    mov dx, cx
    add bx, 1 
    shr dx, 4
    and dx, 0xf 
    call apply_hex_char

    mov dx, cx
    add bx, 1 
    and dx, 0xf 
    call apply_hex_char
    ;print manipulated HEX_OUT string
    mov bx, HEX_OUT
    call print_str
    ret
hex_to_ascii: ;0-F
    cmp al, 10
    jge hex_to_ascii_letter
    add al, 0x30
    jmp hex_to_ascii_end
hex_to_ascii_letter:
    sub al, 10
    add al, 0x41
hex_to_ascii_end: ; do i need this or can i just ret from other condition    ?
    ret
apply_hex_char:
    mov al, dl
    and al, 0xf
    call hex_to_ascii
    mov [bx], al
    ret
HEX_OUT:
    db '0x0000',0
[bits 32]

global load_gdt

load_gdt:
    cli
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.gdt_loaded


.gdt_loaded:
    ret
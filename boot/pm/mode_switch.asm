[bits 16]
switch_to_pm:
    cli ; disable itrps
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1 ; to switch set first bit of cr0 to 1
    mov cr0, eax
    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax ; point segment regs to data selector defined in GDT
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000 ; move stack position to top of free space
    mov esp, ebp
    call BEGIN_PM


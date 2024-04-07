[bits 32]

%macro isr_err_stub 1
isr_stub_%+%1:
    cli ; disable itrps
    push %1 ;push intrp number
    jmp isr_shared_stub
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    cli 
    push 0 ;push fake error code
    push %1
    jmp isr_shared_stub
%endmacro


%macro irq 2
  global irq_%1
  irq_%1:
    cli
    push byte 0 ;dunno
    push byte %2
    jmp irq_shared_stub
%endmacro

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
irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i 
%assign i i+1 
%endrep
%assign i 0 
%rep    16 
    dd irq_%+i 
%assign i i+1 
%endrep



extern exception_handler ; c handler
extern isr_handler
extern irq_handler

isr_shared_stub:
    pusha ; save edi si ebp ebx edx ecx eax
            ; is that the order
    mov ax, ds ; lower 16bits of eax = ds

 ; we save all this stuff for later when itrps can be called from user mode
    push eax    ; save data segment descriptor
    mov ax, 0x10 ; load kernel data segment descriptor 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;start of our struct
    call isr_handler

    pop ebx ; reload data segment descriptor from before
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa ; return regs to state on call
    add esp, 8 ; cleans up the pushed error code and isr number (uhhh)
    sti ; renable intrps
    iret ; pops 5 things at once, cs, eip, eflags, ss, esp


irq_shared_stub:
    pusha ; save edi si ebp ebx edx ecx eax
            ; is that the order
    mov ax, ds ; lower 16bits of eax = ds

                ; we save all this stuff for later when itrps can be called from user mode
    push eax    ; save data segment descriptor
    mov ax, 0x10 ; load kernel data segment descriptor 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler

    pop ebx ; reload data segment descriptor from before
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa ; return regs to state on call
    add esp, 8 ; cleans up the pushed error code and isr number (uhhh)
    sti ; renable intrps
    iret ; pops 5 things at once, cs, eip, eflags, ss, esp


global load_idt
load_idt:
    mov eax, [esp+4] ;ptr to idt passed 
    lidt [eax]
    ret
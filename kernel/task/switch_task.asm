[bits 64]

global capture_regs:function

capture_regs:

     ; Save stack-related registers
    mov [rdi], rsp          ; rsp
    mov [rdi + 8], rbp      ; rbp

    ; Save instruction pointer WRONG WRONG WRONG
    push rax
    call next_instruction
next_instruction:
    pop rax
    mov [rdi + 16], rax     ; rip
    pop rax

    ; Save general-purpose registers
    mov [rdi + 24], rax     ; rax
    mov [rdi + 32], rbx     ; rbx
    mov [rdi + 40], rcx     ; rcx
    mov [rdi + 48], rdx     ; rdx
    mov [rdi + 56], rsi     ; rsi
    mov [rdi + 64], rdi     ; rdi
    mov [rdi + 72], r8      ; r8
    mov [rdi + 80], r9      ; r9
    mov [rdi + 88], r10     ; r10
    mov [rdi + 96], r11     ; r11
    mov [rdi + 104], r12    ; r12
    mov [rdi + 112], r13    ; r13
    mov [rdi + 120], r14    ; r14
    mov [rdi + 128], r15    ; r15

    pushfq
    pop rax
    mov [rdi + 136], rax    ; rflags

    mov rax, cr3
    mov [rdi + 144], rax    ; cr3

    ret

global restore_regs:function

restore_regs:
    pop rax

    ; Restore flags
    push qword [rdi + 136]  ;  rflags
    popfq 

     ; cr3
  
    mov rax, [rdi + 16]     ; load saved rip into rax
   
    mov rsp, [rdi]          ; rsp
    mov rbp, [rdi + 8]      ; rbp
    push rax   ; new RIP


    mov rax, [rdi + 24]     ; rax
    mov rbx, [rdi + 32]     ; rbx
    mov rcx, [rdi + 40]     ; rcx
    mov rdx, [rdi + 48]     ; rdx
    mov rsi, [rdi + 56]     ; rsi
    mov rdi, [rdi + 64]     ; rdi
    mov r8,  [rdi + 72]     ; r8
    mov r9,  [rdi + 80]     ; r9
    mov r10, [rdi + 88]     ; r10
    mov r11, [rdi + 96]     ; r11
    mov r12, [rdi + 104]    ; r12
    mov r13, [rdi + 112]    ; r13
    mov r14, [rdi + 120]    ; r14
    mov r15, [rdi + 128]    ; r15

                    
    

   
    ret ; good fuckin luck
    
   



global fix_isr_regs:function

fix_isr_regs:
    mov rax, 0xffff
    mov rbx, 0xffff
  
    mov rdx, rax
   mov rdi, rax
    int 13



global jump_usermode

asm_jump_usermode:
	mov ax, 0x1b
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov rax, rsp
	push 0x43
	push rax
	pushf
	push 0x3b
	push rdi

	iretq
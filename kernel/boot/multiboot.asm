;default rel
[bits 64]
MB_MAGIC equ 0xe85250d6
MB_ARCH equ 0
;https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Examples

KERNEL_VMA equ 0xffffffff80000000 ;p4[511] p3[510]  0 p2 0
PAGE_SIZE equ 0x200000
[extern _start]
[extern _end]
[extern _edata]
[extern _bss]
[extern ebss]

[extern main]

global p2_table
global p3_table
global p4_table
global MB0
global MB1
global gdt64
global GDT_TSS_PTR
global GDT_PTR ;literally stores address of GDT
global kernel_stack
global kernel_stack_end

section .multiboot_header ;gnu .long = dd gnu .word = dw
align 8
mb_header_start:
    dd MB_MAGIC ; magic 
    dd MB_ARCH ; arch
    dd mb_header_end - mb_header_start

    dd 0x100000000 - (MB_MAGIC + MB_ARCH + (mb_header_end - mb_header_start)) ;checksum

align 8
framebuffer_tag_start:
    dw 5 ; mb2 header tag framebuf
    dw 1 ; header tag optional
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 1024 ;w 640
    dd 768 ; h 400
    dd 32 ; depth
framebuffer_tag_end:
align 8
    dw 0; mb header tag end
    dw 0; flags
    dd 8 ; size
mb_header_end:


global start
section .text
[bits 32]

start:
    cli 
    push 0
    popf

    mov dword [0xfc000000], 0xfffffff
   
    mov dword [MB0 - KERNEL_VMA], ebx
    mov dword [MB1 - KERNEL_VMA], eax

   ; xor eax, eax
   ; mov edi, (_bss - KERNEL_VMA)
   ; mov ecx, (ebss - KERNEL_VMA)

   ; sub ecx, edi
   ; cld
   ; rep stosb
     
    

    ; SETUP ID MAP 
    mov eax, p3_table - KERNEL_VMA
    or eax, 0b11
    mov dword [p4_table - KERNEL_VMA], eax ;qword
    
    mov eax, p2_table - KERNEL_VMA
    or eax, 0b11
    mov dword [p3_table - KERNEL_VMA], eax ;qword

    mov ecx, 0

    mov ebx, (p2_table - KERNEL_VMA)
    .id_map_p2:
        mov eax, PAGE_SIZE
        mul ecx
        or eax, 0b10000011 ;rw avail kernel
        mov [(p2_table - KERNEL_VMA) + ecx * 8], eax
        inc ecx
        cmp ecx, 16; map n * 2mib of memory 
        jne .id_map_p2

    ; MAP HIGHER HALF
    mov eax, (p3_higher - KERNEL_VMA)
    or eax, 0b11
    mov dword [(p4_table - KERNEL_VMA) + 8 * 511], eax ;qword
    
    mov eax, (p2_higher - KERNEL_VMA)
    or eax, 0b11
    mov dword [(p3_higher - KERNEL_VMA) + 8 * 510], eax ;qword

    mov ecx, 0

    .hi_map_p2:
        mov eax, PAGE_SIZE
        mul ecx
        or eax, 0b10000011
        mov [(p2_higher - KERNEL_VMA) + ecx * 8], eax
        inc ecx
        cmp ecx, 512 ; map n * 2mib of memory 
        jne .hi_map_p2

    mov eax, (p4_table - KERNEL_VMA)
    mov cr3, eax

    
     ; enable paging modes 
    mov eax, cr4
    or eax, 1 << 4 ;pse
    or eax, 1 << 5 ; pae
    mov cr4, eax    

     ; set the long mode bit
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 16
    mov cr0, eax
    
    ; load initial gdt
    
    ; update selectors
   
    
   
 
    
 
    mov ebx, (gdt64- KERNEL_VMA)
    mov eax, (gdt64.pointer - KERNEL_VMA)
    lea edx, (gdt64.pointer - KERNEL_VMA)
    lgdt [gdt64.pointer - KERNEL_VMA]

    
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax

   

    jmp gdt64.code:(long_mode_redirect - KERNEL_VMA)


start_hang:
    hlt
    jmp $


[bits 64]
section .text

;[extern _init:function];
;[extern _fini:function]


long_mode_start:
    
      ;set up our stack
    mov rbp, stack_top
    mov rsp, rbp

    

   ;call _init ; does this even do anything (NO!)


    


    mov rax, gdt64
    mov qword [gdt64.pointer + 2], rax
    lgdt [gdt64.pointer]
   

    mov qword rax, qword 0x0
    mov qword [p4_table - KERNEL_VMA], rax

    invlpg [rax] ; un ID map for better or worse

    ;mov qword rax, cr3
    ;mov qword cr3, rax

    mov rdi, [MB0]
    mov rsi, [MB1]
    add rdi, KERNEL_VMA ; add higher map to mb header since its mapped from 0x0 still

    mov rax, main
    
    ; hlt
    ;jmp $

    call main
    

    ;global destructors
   ;call _fini ;broken for now (havent fixed with new toolchain)
    cli 
    hlt
    jmp $

long_mode_redirect:
    ;try and force long non rel jmp
    mov qword rax, long_mode_start
    jmp rax


section .data
   
    GDT_PTR dq gdt64
    GDT_TSS_PTR dq tss_offset
    kernel_stack dq stack_top
    kernel_stack_end dq stack_bottom

    MB0 dq 0x00000000000
    MB1 dq 0x00000000000
    


    
    

section .bss
    align 4096
    stack_bottom:
        resb 16384 
    stack_top:
    ;so we only wanna use this for isrs etc eventually

    reserved:
        resb 8192
    

    align 4096
    p4_table: ;page map table
        resb 4096
    p3_table: ;page directory pointer table
        resb 4096
    p2_table: ; page directory table
        resb 4096

    align 4096
    p2_higher:
        resb 4096
    p3_higher:
        resb 4096
   
   
    
   

    


[bits 64]
section .rodata


;intel manual pg. 3098
;https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
 ; flag defs moved to GDT.h for ref
align 16
gdt64: 
    dq 0 ;null segment
; KERNEL CODE 0x8
code_offset equ $ - gdt64
.code: equ $ - gdt64 ;0x8
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
; KERNEL DATA 0x10
data_offset equ $ - gdt64
.data: equ $ - gdt64 ;0x10
    dq (1<<44) | (1<<47) | (1<<41)
; USER CODE 0x18 | 0x3 = 0x1b
user_code_offset equ $ - gdt64
.user_code: equ $ - gdt64 ;0x18
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) | (1<<46) | (1<<45) ;45/6 user prv
; USER DATA 0x20 | 0x3 = 0x23 
user_data_offset equ $ - gdt64
.user_data: equ $ - gdt64 ;0x20 
    dq (1<<44) | (1<<47) | (1<<41) | (1<<46) | (1<<45) ;45/6 usr prv
; TASK STATE SEGMENT 0x28
tss_offset equ $ 
.tss_entry: equ $ - gdt64
   dq 0
   dq 0
.pointer:
    dw .pointer - gdt64 - 1 
    dq (gdt64 - KERNEL_VMA)



section .text
bits 64

global load_tss:function
load_tss:
    ;lgdt [gdt64.pointer]
    
    ; load TSS
    mov ax, gdt64.tss_entry
    ltr ax

    ret
    
   
 
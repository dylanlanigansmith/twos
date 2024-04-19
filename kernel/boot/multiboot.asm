MB_MAGIC equ 0xe85250d6
MB_ARCH equ 0
;https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Examples

[extern _end]
[extern bss]

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
    dd 640
    dd 400
    dd 8
framebuffer_tag_end:
align 8
    dw 0; mb header tag end
    dw 0; flags
    dd 8 ; size
mb_header_end:

[bits 32]

global start
section .text
[extern main]


HIGH_START equ 0xffffff8000000000 ;p4[511] p3  0 p2 0
PAGE_SIZE equ 0x200000
start:
    mov [MB0], ebx
    mov [MB1], eax
  
    ; reset eflags
    push 0
    popf
    ;push multiboot stuff to stack
    ;;push ebx
    ;;push eax
    mov [0xA0008], byte 0x1

    ;https://intermezzos.github.io/book/first-edition/paging.html
     ; Point the first entry of the level 4 page table to the first entry in the
    ; p3 table
    mov eax, p3_table
    or eax, 0b11 ;
    mov dword [p4_table + 0], eax

    ; Point the first entry of the level 3 page table to the first entry in the
    ; p2 table
    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table + 0], eax
    
 

    ; point each page table level two entry to a page
    mov ecx, 0         ; counter variable
.map_p2_table:
    mov eax, 0x200000  ; 2MiB
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512 ; map 512 * 2mib of memory = 1gib total
    jne .map_p2_table


   
    mov eax, p3_higher
    or eax, 0b11 ;
    mov dword [p4_table + 8 * 511], eax

    mov eax, p2_higher
    or eax, 0b11
    mov dword [p3_higher + 0], eax


    mov ecx, 0         ; counter variable
.map_high_table:
    mov eax, 0x200000  ; 2MiB
    mul ecx
    or eax, 0b10000011
    mov [p2_higher + ecx * 8], eax;

    inc ecx
    cmp ecx, 32 ; map 32 * 2mib of memory = 64mib
    jne .map_high_table


    ; move page table address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; enable PAE
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

    ; load gdt (warning that im gonna have 2 fix IDT stuff for 64bit)
    lgdt [gdt64.pointer]
    ; update selectors
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax
    
    
   ;jmp to long mode 
   jmp gdt64.code:long_mode_start


global GDT_TSS_PTR
global GDT_PTR ;literally stores address of GDT
global kernel_stack
global kernel_stack_end
section .data
    MB0 dq 0x00000000000
    MB1 dq 0x00000000000
    GDT_PTR dq gdt64
    GDT_TSS_PTR dq tss_offset
    kernel_stack dq stack_top
    kernel_stack_end dq stack_bottom
global p2_table
global p2_table2
global p3_table
global p4_table



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

section .rodata






gdt64: 
;intel manual pg. 3098
;https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
 ; flag defs moved to GDT.h for ref
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
        
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) | (1<<46) | (1<<45)

; USER DATA 0x20 | 0x3 = 0x23 
user_data_offset equ $ - gdt64
.user_data: equ $ - gdt64 ;0x20 
    dq (1<<44) | (1<<47) | (1<<41) | (1<<46) | (1<<45)

; TASK STATE SEGMENT 0x28
tss_offset equ $ 
.tss_entry: equ $ - gdt64
;tss addr = base ; tss size = limit ; 0x89 p/e/a as access and 0x40 size bit as flags <- unknown re: size bit
   dq 0;( tss_size & 0xffff ) | ( (task_state_segment & 0xffffff) << 16) | (0x89 << 40) | ( (tss_size & 0xff0000 ) << 48) | (0x40 << 52) | ( ( task_state_segment & 0xff000000) << 56  )
   dq 0;(task_state_segment & 0xffffffff00000000)

; GTDR
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64






section .text
[bits 64]
[extern _init:function]
[extern _fini:function]


long_mode_start:
   
    ;mov rax, 0x2f592f412f4b2f4f
    ;mov qword [0xb8000], rax
    ;pray multiboot stuff still kicking around on stack
      ;set up our stack
    mov rbp, stack_top
    mov rsp, rbp

   

 

    call _init ; does this even do anything (NO!)

;     lgdt [gdt64.pointer]
   ; push qword [MB0]
   ; push qword [MB1]
    mov edi, [MB0]
    mov esi, [MB1]
    call main
    

    ;global destructors
    call _fini ;broken for now (havent fixed with new toolchain)
    cli 
    hlt
    jmp $


global load_tss:function

load_tss:
    ;reload gdt ?
    lgdt [gdt64.pointer]
    ; update selectors
   ; mov ax, gdt64.data
   ; mov ss, ax
  ;  mov ds, ax
   ; mov es, ax

     ; load TSS
    mov ax, gdt64.tss_entry
    ltr ax
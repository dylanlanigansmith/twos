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
    
   ; mov eax, p2_table2
   ; or eax, 0b11
   ; mov dword [p3_table + 8], eax

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


   
;
;   mov ecx, 0         ; counter variable
;.map_p2_table2:
;    mov eax, 0x200000  ; 2MiB
;    mul ecx
;    or eax, 0b10000011
;    mov [p2_table + ecx * 8], eax;

;    inc ecx
;    cmp ecx, 512 ; map 512 * 2mib of memory = 1gib total
;    jne .map_p2_table2


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

global GDT_CODE_OFFSET
global GDT_DATA_OFFSET
section .data
    MB0 dq 0x00000000000
    MB1 dq 0x00000000000
    GDT_CODE_OFFSET dd code_offset
    GDT_DATA_OFFSET dd data_offset

global p2_table
global p2_table2
global p3_table
global p4_table
section .bss
    align 4096
    stack_bottom:
        resb 16384 
    stack_top:

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
    p2_table2:
        resb 4096

section .rodata
gdt64:
    dq 0
code_offset equ $ - gdt64
.code: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
data_offset equ $ - gdt64
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41)
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64


section .text
[bits 64]
long_mode_start:
   
    ;mov rax, 0x2f592f412f4b2f4f
    ;mov qword [0xb8000], rax
    ;pray multiboot stuff still kicking around on stack
      ;set up our stack
    mov rbp, stack_top
    mov rsp, rbp

   ; push qword [MB0]
   ; push qword [MB1]
    mov edi, [MB0]
    mov esi, [MB1]
    call main
    
    cli 
    hlt
    jmp $
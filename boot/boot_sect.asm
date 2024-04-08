;compile w/: 
;   nasm boot_sect.asm -f bin -o boot_sect.bin 
; https://www.cs.bham.ac.uk/%7Eexr/lectures/opsys/10_11/lectures/os-dev.pdf 

; chapter 5 start
[org 0x7c00] ;boot sector loaded at 0x7c00 by bios, offset addresses as such for labels (just labels?)

KERNEL_OFFSET equ 0x1000


mov bp, 0x9000 ; set stack above where we are loaded 
mov sp, bp


mov bx, BOOT_MSG
call print_str

call load_kernel

;call clear_screen_real

;set wanted graphics mode for kernel
mov al, 0x13 ;0x3 for text 13 for gfx
int 0x10

call switch_to_pm ; bye!

jmp $ ;jump forever


%include "16bit/screen/print_str_rm.asm"
%include "16bit/screen/print_hex_rm.asm"
%include "16bit/screen/clear_screen_rm.asm"
%include "16bit/disk/disk_load.asm"

%include "pm/gdt.asm"
%include "pm/mode_switch.asm"
%include "pm/print_str_pm.asm"



[bits 16]
load_kernel:
    mov bx, LOAD_MSG
    call print_str

    mov bx, KERNEL_OFFSET
    mov dh, 48 ; we want the first 15 sectors (minus boot sector) from boot disk 
                ; boot disk should be our kernel code
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret
    

[bits 32]
BEGIN_PM:
    ;if in console gfx print otherwise no!
   ; mov ebx, PROT_MODE_MSG
   ; call print_str_pm
    call KERNEL_OFFSET
    jmp $





; strings for logs and error messages
BOOT_MSG    db 'Entered 16-bit Real Mode',0
LOAD_MSG    db 'Loading Kernel...',0
PROT_MODE_MSG db 'bootstrapping',0





; global stuff

BOOT_DRIVE: db 0





; padding and boot sector stuff
times 510-($-$$) db 0 ; must fit into 512 bytes, tell compiler to pad us to 510th
dw 0xaa55 ;magic number that says we a boot sector


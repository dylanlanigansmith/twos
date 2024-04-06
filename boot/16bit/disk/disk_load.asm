[bits 16]

disk_load:
    push dx ; dh = how many sectors to read, store on stack for later
            ; dl = boot drive to use
    mov ah, 0x02 ; bios read sector fn
    mov al, dh ; # sectors to read
    mov ch, 0x00 ; select cylinder 0 
    mov dh, 0x00 ; select head 0
    mov cl, 0x02 ; start reading from second sector (so after boot sector)
    int 0x13 ; call bios interupt
    jc disk_error ; carry flag set if interupt has error

    pop dx ; restore dx to see if we read expected # of sectors
    cmp dh, al ; al = sectors actually read
    jne sector_error
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_str
    jmp $ ; hang after displaying error

sector_error:
    push ax ;save for printing sector info
    push dx ; ^ 
    mov bx, SECTOR_ERROR_MSG
    call print_str
    pop dx
    pop ax
    mov dl, al ; so dh = sectors wanted dl = al = sectors read  (in theory at least)
                ; we probably wont even get an error to test this out so that is cool
    call print_hex
    
    jmp $ ; hang to display error


DISK_ERROR_MSG  db "Disk Read Error!",0

SECTOR_ERROR_MSG    db "Sectors read mismatch (0x wanted returned)  " ; will print dh vs al after 
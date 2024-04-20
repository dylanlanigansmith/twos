
ENTRY(start)


KERNEL_LMA = 0x0000000000200000;

VIRT_ADDR = 0xffffffff80000000;
        
KERNEL_VMA = VIRT_ADDR + KERNEL_LMA;

SECTIONS {
    . = KERNEL_VMA;

    _start = .;
    .boot :  AT(ADDR(.boot) - VIRT_ADDR)
    {
        /* ensure that the multiboot header is at the beginning */
        *(.multiboot_header)
        . = ALIGN(4K);
    }
    
     _kernel_start = .;
    .text : AT(ADDR(.text) - VIRT_ADDR)
    {
        code = .; _code = .; __code = .;
        *(.text)
         

        *(.gnu.linkonce.b*)
        . = ALIGN(4K);
    }
    
	/* Read-write data (initialized) */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - VIRT_ADDR)
    {
        _rdata = .;
       *(.rodata .rodata*)
       
        *(.gnu.linkonce.r*)
        
    } 
    
    . += 2M;
    /* removed BLOCK(4K) for this and bss */
	.data : AT(ADDR(.data) - VIRT_ADDR)
	{
        _data = .;
        
		*(.data .data*)
        *(.gnu.linkonce.r*)
        . = ALIGN(4K);
	}
    _edata = .;
	/* Read-write data (uninitialized) and stack */
	.bss : AT(ADDR(.bss) - VIRT_ADDR)
	{
        bss = .; _bss = .; __bss = .;
		*(COMMON)
		*(.bss)
        
        ebss = .;
        *(.gnu.linkonce.b*)
        . = ALIGN(4K);
	}

    end = .; _end = .; __end = .; _kernel_end = .;

  
   
    /DISCARD/ :
    {
        *(.comment)
        *(note.*)
        *(.eh_frame) /* discard this, unless you are implementing runtime support for C++ exceptions. */
    }
}
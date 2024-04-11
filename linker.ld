ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .text ALIGN(4K) :
    {
        code = .; _code = .; __code = .;
        *(.text)
        
       /* . = ALIGN(4K);*/
        *(.gnu.linkonce.b*)
    }
 
	/* Read-write data (initialized) */
    .rodata ALIGN(4K) : {
        *(.init)
        *(.fini)

        *(.rodata)
        *(.gnu.linkonce.r*)
    }
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
        *(.gnu.linkonce.r*)
	}
    
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
        bss = .; _bss = .; __bss = .;
		*(COMMON)
		*(.bss)
        ebss = .;
        *(.gnu.linkonce.b*)
	}

    end = .; _end = .; __end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame) /* discard this, unless you are implementing runtime support for C++ exceptions. */
    }
}
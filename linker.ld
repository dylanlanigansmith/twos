ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .text :
    {
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(4K);
    }
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
        *(.rodata)
	}
    
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
        bss = .; _bss = .; __bss = .;
		*(COMMON)
		*(.bss)
        ebss = .;
	}

    end = .; _end = .; __end = .;
}
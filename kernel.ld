ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS {
	. = 0x8400;
	.text :
	{
        *(.boot)
		*(.text)
	}

    .rodata :
    {
    	*(.rodata)
    }

    .data :
    {
    	*(.data)
    }

    .bss :
    {
    	*(.bss)
    }
}

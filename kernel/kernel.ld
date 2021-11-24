ENTRY(_start)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 1M;
    .text : {
        *(.boot)
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}

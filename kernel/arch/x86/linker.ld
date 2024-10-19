ENTRY(_start)
SECTIONS
{
    . = 1M;
   .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        *(.multiboot2_header)
    }

    .text :
    {
        *(.text)
    }

    .asm :
    {
        *(.asm)
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
        *(COMMON)
        *(.bss)
    }

    .initial_stack :
    {
        *(.initial_stack)
    }
}

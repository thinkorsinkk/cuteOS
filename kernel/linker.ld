/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

/* We want the symbol _start to be our entry point */
ENTRY(_start)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
    . = 0xffffffff80000000;

    PROVIDE(TEXT_START = .);
    .text : {*(.text*)}
    PROVIDE(TEXT_END = .);

    . += CONSTANT(MAXPAGESIZE);

    PROVIDE(RODATA_START = .);
    .rodata : {*(.rodata*)}
    PROVIDE(RODATA_END = .);

    . += CONSTANT(MAXPAGESIZE);

    PROVIDE(DATA_START = .);
    .data : {*(.data*)}
    PROVIDE(DATA_END = .);

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}

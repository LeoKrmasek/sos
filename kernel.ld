ENTRY(_kernel_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
	. = 0xC0100000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000) ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000) ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}

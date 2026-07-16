ENTRY(_entry);
 
. = 0x80000000;
 
SECTIONS {
    PROVIDE(_kernel_physical_start = .);
	/* Include entry point at start of binary */
	.text : ALIGN(4K) {
		KEEP(*(.entry*));
		*(.text);
	}
	.bss : ALIGN(4K) {
		PROVIDE(bss_start = .);
		*(.bss);
		. += 4096;
		PROVIDE(stack_top = .);
		. += 4096;
		PROVIDE(__global_pointer = .);
		PROVIDE(bss_end = .);
	}

	.rodata : ALIGN(4K) {
		*(.rodata) *(.rodata*);
        *(.srodata) *(.srodata*);
	}
	.data : ALIGN(4K) {
		*(.data);
	}
    PROVIDE(_kernel_physical_end = .);
}

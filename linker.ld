ENTRY(_entry);
 
. = 0x80000000;
 
SECTIONS {
	/* Include entry point at start of binary */
	.text : ALIGN(4K) {
		KEEP(*(.init*));
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
}

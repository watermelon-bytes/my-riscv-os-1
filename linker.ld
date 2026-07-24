ENTRY(_entry);
 
. = 0x80000000;
PROVIDE(_kernel_physical_start = .);
SECTIONS {
	/* Include entry point at start of binary */
	.text : ALIGN(4K) {
		KEEP(*(.entry*));
		*(.text);
	}
	.bss : ALIGN(4K) {
		PROVIDE(bss_start = .);
		*(.bss) *(.bss*);
        *(.sbss) *(.sbss*);
		. += 4K;
		PROVIDE(stack_top = .);
		. += 8K;
		PROVIDE(__global_pointer = .);
		PROVIDE(bss_end = .);
	}

	.rodata : ALIGN(4K) {
		*(.rodata) *(.rodata*);
        *(.srodata) *(.srodata*);
	}
	.data : ALIGN(4K) {
		*(.data);
        . = ALIGN(4K);
	}
    PROVIDE(_kernel_physical_end = .);
}

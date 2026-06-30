.align 4
.section .init

.type _entry, @function
.global _entry
_entry:
    csrw satp, zero 
	la gp, __global_pointer
    la sp, stack_top
    call kmain 
1:
    wfi
    j 1b

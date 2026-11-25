.align 4
.section .entry

.type _entry, @function
.global _entry
_entry:
	la gp, __global_pointer
    la sp, stack_top
    csrw satp, zero
    call early_start
1:
    wfi
    j 1b

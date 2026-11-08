#include <klibc/types.h>

// Any __riscv_xlen compatible per-process context.
// Stores saved (`s0`-`s11`) registers plus stack pointer and return address.
// Used when handling `ecall`.
struct per_process_context {
    reg_t ret_addr;
    reg_t stack_ptr;
    reg_t saved_registers[12];
};

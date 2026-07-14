#pragma once
#include <types.h>

// clang-format off
enum riscv_extensions : u32 {
    EXT_ATOMIC              = 1u << 0,
    EXT_BIT_MANIPULATION    = 1u << 1,
    EXT_COMPRESSED          = 1u << 2,
    EXT_DOUBLE_PREC_FPU     = 1u << 3,
    EXT_LESS_REGISTERS      = 1u << 4,
    EXT_FLOATING_POINT      = 1u << 5,
    EXT_MULTIPLY_N_DIVIDE   = 1u << 12,
    EXT_QUADRA_PREC_FPU     = 1u << 16,
    EXT_SUPERVISOR_MODE     = 1u << 18,
    EXT_USER_MODE           = 1u << 20,
    EXT_VECTOR              = 1u << 21,
    EXT_NON_STANDARD        = 1u << 23
};
// clang-format on

// Reads Machine ISA Register and if bit 20 (U-mode implemented) is set, returns
// non-zero value
// Returns 0 if user mode is not present on the current machine
int is_extension_implemented(enum riscv_extensions);

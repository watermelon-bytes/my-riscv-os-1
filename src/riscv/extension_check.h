#pragma once
#include <stdbool.h>
#include <types.h>

// clang-format off
enum riscv_extensions : u32 {
    RISCV_EXT_ATOMIC              = 1u << 0,
    RISCV_EXT_BIT_MANIPULATION    = 1u << 1,
    RISCV_EXT_COMPRESSED          = 1u << 2,
    RISCV_EXT_DOUBLE              = 1u << 3,
    RISCV_EXT_LESS_REGS           = 1u << 4,
    RISCV_EXT_FLOAT               = 1u << 5,
    RISCV_EXT_MUL_DIV             = 1u << 12,
    RISCV_EXT_QUAD                = 1u << 16,
    RISCV_EXT_SUPERVISOR_MODE     = 1u << 18,
    RISCV_EXT_USER_MODE           = 1u << 20,
    RISCV_EXT_VECTOR              = 1u << 21,
    RISCV_EXT_NON_STANDARD        = 1u << 23
};
// clang-format on

// Reads Machine ISA Register and if the specified extension's bit is set,
// returns non-zero value
// Returns 0 if the specified extension is not present on
// the current machine
bool is_extension_implemented(enum riscv_extensions);

// Calls kernel panic the CPU if `is_extension_implemented` returns false on
// any of `required_riscv_extension`s
void ensure_extensions_present(const register_t req);

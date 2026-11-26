#pragma once
enum riscv_privileges : unsigned {
    RISCV_PRIV_LVL_USER = 0,
    RISCV_PRIV_LVL_SUPERVISOR = 1,
    RISCV_PRIV_LVL_MACHINE = 3
};

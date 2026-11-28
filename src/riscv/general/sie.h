#pragma once

enum : unsigned {
    /* Enables Supervisor software interrupts */
    RISCV_SIE_SSIE = 1u << 1,

    /* Enables Supervisor timer interrupts */
    RISCV_SIE_STIE = 1u << 5,

    /* Enables Supervisor external interrupts */
    RISCV_SIE_SEIE = 1u << 9,

    /* Enables Supervisor local counter overflow interrupts (requires the
       Sscofpmf extension) */
    RISCV_SIE_LCOFIE = 1u << 13,
};

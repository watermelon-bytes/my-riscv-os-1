#pragma once
#include <types.h>

enum riscv_interrupt_sources_ : reg_t {
    // Machine-level interrupts:
    INTSRC_MACHINE_SOFTWARE = 1u << 3,
    INTSRC_MACHINE_TIMER = 1u << 7,
    INTSRC_MACHINE_EXTERNAL = 1u << 11,

    // Supervisor-level:
    INTSRC_SUPERVISOR_SOFTWARE = 1u << 1,
    INTSRC_SUPERVISOR_TIMER = 1u << 5,
    INTSRC_SUPERVISOR_EXTERNAL = 1u << 9,

    INTSRC_LOCAL_COUNTER_OVERFLOW = 1u << 13,
};

// All valid bits for `mip`/`mie` enabled.
const reg_t RISCV_ALL_INTR_SOURCES_ON =
    INTSRC_MACHINE_EXTERNAL | INTSRC_MACHINE_TIMER | INTSRC_MACHINE_SOFTWARE |
    INTSRC_SUPERVISOR_EXTERNAL | INTSRC_SUPERVISOR_SOFTWARE |
    INTSRC_SUPERVISOR_SOFTWARE;

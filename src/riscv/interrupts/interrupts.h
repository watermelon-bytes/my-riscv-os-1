#pragma once
#include <stdbool.h>
#include <types.h>
/*
 * Detects an external interrupt controller
 */
void init_interrupt_controller(const void* device_tree);

/*
 * Clears the MIE bit in the mstatus CSR, thereby disabling interrupts globally.
 */
void disable_interrupts();

/*
 * Sets the MIE bit in mstatus, enabling interrupts
 */
void enable_interrupts();

enum riscv_privileges {
    RISCV_PRIV_LVL_USER,
    RISCV_PRIV_LVL_SUPERVISOR,
    RISCV_PRIV_LVL_MACHINE = 3
};

union mstatus_32 {
    uintptr_t raw_value_;
    // I prefer to use more verbose naming. Maybe I shouldn't do so.
    __attribute__((packed)) struct {
        u32 : 1;  // reserved
        u32 supervisor_interrupt_enable : 1;
        u32 : 1;
        u32 machine_interrupt_enable : 1;
        u32 user_prev_interrupt_enable : 1;
        u32 supervisor_prev_interrupt_enable : 1;
        u32 : 1;
        u32 machine_prev_interrupt_enable : 1;
        u32 supervisor_prev_privilege : 1;
        u32 vector_state : 2;
        u32 machine_prev_privilege : 2;
        u32 floating_point_state : 2;
        u32 extension_state : 2;
        u32 modify_privilege : 1;
        u32 supervisor_user_memory_access : 1;
        u32 make_exec_readable : 1;
        u32 trap_virtual_memory : 1;
        u32 timeout_wfi_instr : 1;
        u32 trap_sret : 1;
    };
};

union mstatush_32 {};

union mstatus_64 {
    u64 raw_value;
    struct {
        union mstatus_32 lo;
        union mstatush_32 hi;
    };
};

union mtrap_vector {
    register_t raw_value;
    struct {
        enum mtvec_mode_field {
            MTVEC_MODE_DIRECT,
            MTVEC_MODE_VECTORED
        } mode : 2;
        register_t : 0;
    };
    void* base;
};

_Static_assert(sizeof(union mtrap_vector) == sizeof(register_t),
               "Poorly formatted `mtvec` fields (in source file " __FILE_NAME__
               ")\n");

void setup_interrupt_handler();

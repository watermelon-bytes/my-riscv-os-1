#pragma once
#include <klibc/types.h>
#include <stdbool.h>
/*
 * Detects an external interrupt controller and initializes it.
 *
 * Panics if can't find a well-known interrupt controller in the Device Tree,
 * such as Core-Local Interrupt Controller (CLINT).
 */
void init_interrupt_controller(const void* device_tree);

/*
 * Clears the MIE bit in the mstatus CSR, thereby disabling interrupts globally.
 */
void disable_interrupts();

/*
 * Sets the MIE bit in mstatus, enabling interrupts.
 */
void enable_interrupts();

/* stvec / mtvec – identical layout */
union riscv_trap_vector {
    register_t raw_value;

    struct {
        enum {
            TRAP_VECTOR_MODE_DIRECT = 0,
            TRAP_VECTOR_MODE_VECTORED = 1
            /* ≥2 reserved */
        } mode : 2;

        /* BASE is [XLEN-1:2]; must be 4-byte aligned (stricter for vectored) */
        reg_t base : (sizeof(reg_t) * 8 - 2);
    };

    /* Convenience: the whole register as a pointer (mode bits are low) */
    void* base_ptr;
};

_Static_assert(sizeof(union riscv_trap_vector) == sizeof(register_t) &&
                   sizeof(register_t) == sizeof(void*),
               "Poorly formatted `riscv_trap_vector` fields (" __FILE_NAME__
               ")\n");

/*
 * NOTE: This function does not touch global interrupts state.
 * Interrupts need to be disabled before calling this.
 */
void setup_interrupt_handler();

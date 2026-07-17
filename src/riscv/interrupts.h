#include <stdbool.h>
#include <types.h>
/*
 * Detects an external interrupt controller
 */
void init_interrupt_controller(void* device_tree);

/*
 * Clears the MIE bit in the mstatus CSR, thereby disabling interrupts globally.
 */
void disable_interrupts();

/*
 * Sets the MIE bit in mstatus, enabling interrupts
 */
void enable_interrupts();

enum {
    MSTATUS_MIE = 1u << 3,
};

enum mtvec_mode_field { MTVEC_MODE_DIRECT, MTVEC_MODE_VECTORED };

union mtrap_vector {
    register_t _raw_value;
    struct {
        register_t mode : 2;
        register_t base : sizeof(uintptr_t) - 2;
    };
};

_Static_assert(sizeof(union mtrap_vector) == sizeof(register_t),
               "Poorly formatted `mtvec` fields (in source file " __FILE_NAME__
               ")\n");

void setup_interrupt_handler();

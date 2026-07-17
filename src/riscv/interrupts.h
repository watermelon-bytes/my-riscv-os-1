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

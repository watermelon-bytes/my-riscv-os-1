#include "interrupt_controller.h"

#include <riscv/csr_operations.h>

void disable_interrupts() {
    __auto_type curr_mstatus_value = READ_CSR(mstatus);
    WRITE_CSR(mstatus, curr_mstatus_value & ~MSTATUS_MIE);
}

void enable_interrupts() {
    __auto_type curr_mstatus_value = READ_CSR(mstatus);
    WRITE_CSR(mstatus, curr_mstatus_value | MSTATUS_MIE);
}

#include <klibc/panic.h>
#include <riscv/csr_operations.h>
#include <riscv/interrupts.h>
void disable_interrupts() {
    __auto_type curr_mstatus_value = READ_CSR(mstatus);
    WRITE_CSR(mstatus, curr_mstatus_value & ~MSTATUS_MIE);
}

void enable_interrupts() {
    __auto_type curr_mstatus_value = READ_CSR(mstatus);
    WRITE_CSR(mstatus, curr_mstatus_value | MSTATUS_MIE);
}

__attribute__((used, noreturn)) void handle() {
    while (true) {
        asm volatile("wfi;");
    }
    __builtin_unreachable();
}

void setup_interrupt_handler() {
    asm volatile(
        "csrw mideleg, zero;"
        "csrw medeleg, zero;");
    ASSERT((uintptr_t)&handle % 4 == 0);
    __auto_type handler_address = (uintptr_t)&handle;
    WRITE_CSR(mtvec, handler_address | MTVEC_MODE_DIRECT);
}

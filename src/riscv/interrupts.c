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

__attribute__((aligned(4), noinline)) void handle() {
    printf("Caught exception! mcause = %i\n", READ_CSR(mcause));
    halt();
}

void setup_interrupt_handler() {
    asm volatile(
        "csrw mideleg, zero;"
        "csrw medeleg, zero;");
    ASSERT_WITH_MSG((uintptr_t)&handle % 4 == 0,
                    "Handler address is not aligned!");
    __auto_type handler_address = (uintptr_t)&handle;
    // Specify handler address
    WRITE_CSR(mtvec, handler_address | MTVEC_MODE_DIRECT);

    // Allow all interrupts
    WRITE_CSR(mie, UINT32_MAX);
}

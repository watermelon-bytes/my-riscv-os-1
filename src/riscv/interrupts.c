#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/utils.h>
#include <libfdt.h>
#include <riscv/clint/clint.h>
#include <riscv/csr_operations.h>
#include <riscv/interrupt_sources.h>
#include <riscv/interrupts.h>
#include <riscv/timer.h>

void disable_interrupts() {
    union mstatus_32 mstatus = {.raw_value_ = READ_CSR(mstatus)};
    mstatus.machine_interrupt_enable = 0;
    WRITE_CSR(mstatus, mstatus.raw_value_);
}

void enable_interrupts() {
    union mstatus_32 mstatus;
    mstatus.raw_value_ = READ_CSR(mstatus);
    mstatus.machine_interrupt_enable = 1;
    WRITE_CSR(mstatus, mstatus.raw_value_);
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
    WRITE_CSR(mie, RISCV_ALL_INTR_SOURCES_ON);
    WRITE_CSR(mip, RISCV_ALL_INTR_SOURCES_ON);
    printf("[OK] Enabled all interrupts\n");
}

struct riscv_timer present_timer;

void init_interrupt_controller(const void* device_tree) {
    if (discover_clint(device_tree) != 0) {
        // TODO:
        present_timer = clint_init_timer();
        printf("[OK] Discovered CLINT\n");
    }
}

#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/utils.h>
#include <libfdt.h>
#include <riscv/clint/clint.h>
#include <riscv/csr_operations.h>
#include <riscv/interrupts/exception_codes.h>
#include <riscv/interrupts/interrupt_sources.h>
#include <riscv/interrupts/interrupts.h>
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
    WRITE_CSR(mip, 0);
    WRITE_CSR(mstatus, mstatus.raw_value_);
}

__attribute__((aligned(4), noinline)) void handle() {
    const word_t cause = READ_CSR(mcause), epc = READ_CSR(mepc),
                 tval = READ_CSR(mtval), status = READ_CSR(mstatus),
                 satp = READ_CSR(satp);

    printf("\n========== TRAP ==========\n");
    printf("mcause = 0x%x\n", cause);
    printf("mepc   = 0x%x\n", epc);
    printf("mtval  = 0x%x\n", tval);
    printf("mstatus= 0x%x\n", status);
    printf("satp   = 0x%x\n", satp);
    printf("mtvec  = 0x%x\n", READ_CSR(mtvec));
    printf("==========================\n");

    halt();
}

void setup_interrupt_handler() {
    asm volatile(
        "csrw mideleg, zero;"
        "csrw medeleg, zero;");
    ASSERT_WITH_MSG((uintptr_t)&handle % 4 == 0,
                    "Handler address is not aligned!");
    union mtrap_vector mtvec;
    mtvec.mode = MTVEC_MODE_DIRECT;
    mtvec.base = &handle;
    // Specify handler address
    WRITE_CSR(mtvec, mtvec.raw_value);
    WRITE_CSR(stvec, mtvec.raw_value);
    ASSERT(READ_CSR(mtvec) == (uintptr_t)&handle);

    // Allow all interrupts
    WRITE_CSR(mie, RISCV_ALL_INTR_SOURCES_ON);
    WRITE_CSR(mip, RISCV_ALL_INTR_SOURCES_ON);
    // printf("[OK] Enabled all interrupts\n");
}

struct riscv_timer present_timer;

void init_interrupt_controller(const void* device_tree) {
    switch (discover_clint(device_tree)) {
        case 0:
            present_timer = clint_init_timer();
            printf("[OK] Discovered CLINT\n");
            printf("[INFO] mtimecmp at %p, mtime at %p\n",
                   present_timer.mtimecmp, present_timer.mtime);
            return;
        case -2:
            KERNEL_PANIC("CLINT node is present but not valid");
        case -1:
            KERNEL_PANIC("No CLINT node in Device Tree found");
    }
}

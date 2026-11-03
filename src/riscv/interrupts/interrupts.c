#include <drivers/power.h>
#include <init_devices.h>
#include <klibc/k_assert.h>
#include <klibc/panic.h>
#include <klibc/utils.h>
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

__attribute__((aligned(4), noinline)) 
/*
 * Temporary interrupt handler.
 * Prints some info on a few system-level registers that describe the interrupt and its cause.
 */
void m_mode_handle() {
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
#ifndef NDEBUG
    shutdown(1);
#else
    halt();
#endif
}

__attribute__((aligned(4), noinline, noreturn))  //
void s_mode_handle() {
    const word_t cause = READ_CSR(scause), epc = READ_CSR(sepc),
                 tval = READ_CSR(stval), status = READ_CSR(sstatus),
                 satp = READ_CSR(satp);

    printf("\n========== SUPERVISOR MODE TRAP ==========\n");
    printf("scause = 0x%x\n", cause);
    printf("sepc   = 0x%x\n", epc);
    printf("stval  = 0x%x\n", tval);
    printf("sstatus= 0x%x\n", status);
    printf("satp   = 0x%x\n", satp);
    printf("stvec  = 0x%x\n", READ_CSR(stvec));
    printf("============================================\n");
#ifndef NDEBUG
    shutdown(1);
#else
    halt();
#endif
    __builtin_unreachable();
}

void setup_interrupt_handler() {
    // TODO: remove hardcoded interrupt redirection.
    // i.e. don't set mideleg and medeleg to all zeroes, at least here in
    // `setup_interrupt_handler`. Now this temporary workaround is necessary
    // because our trap handler is written for M-mode.
    WRITE_CSR(mideleg, 0);
    WRITE_CSR(medeleg, 0);
    // can't static_assert this, unfortunately
    DEBUG_ASSERT((uintptr_t)&m_mode_handle % 4 == 0);

    union riscv_trap_vector mtvec = {.base = &m_mode_handle};
    // Specify handler address
    // WRITE_CSR(mtvec, mtvec.raw_value);
    // WRITE_CSR(stvec, mtvec.raw_value);
    // DEBUG_ASSERT(READ_CSR(mtvec) == (uintptr_t)&m_mode_handle);

    // Allow all interrupts
    // WRITE_CSR(mie, RISCV_ALL_INTR_SOURCES_ON);
    // WRITE_CSR(mip, RISCV_ALL_INTR_SOURCES_ON);
    WRITE_CSR(mie, 0);
    WRITE_CSR(mip, 0);
    printf("[OK] Enabled all interrupts\n");
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

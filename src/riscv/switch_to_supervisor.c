#include <riscv/csr_operations.h>
#include <riscv/interrupts/interrupt_sources.h>
#include <riscv/interrupts/interrupts.h>
#include <riscv/switch_to_supervisor.h>
extern void handle();

void switch_to_supervisor(void* jump_addr) {
    union mstatus_32 mstatus = {.raw_value_ = READ_CSR(mstatus)};

    // mret will switch privilege level to value of mstatus.MPP
    // mstatus.machine_prev_privilege = RISCV_PRIV_LVL_SUPERVISOR;
    // Leave M-mode interrupts enabled:
    mstatus.machine_prev_interrupt_enable = 0;
    WRITE_CSR(pmpaddr0, WORD_MAX);
#if __riscv_xlen == 32
    WRITE_CSR(pmpaddr1, WORD_MAX);
#endif
    WRITE_CSR(pmpcfg0, 0x1F);
    WRITE_CSR(satp, 0);
    WRITE_CSR(mstatus, mstatus.raw_value_);
    WRITE_CSR(mepc, (uintptr_t)jump_addr);
    WRITE_CSR(medeleg, RISCV_ALL_INTR_SOURCES_ON);
    WRITE_CSR(mideleg, RISCV_ALL_INTR_SOURCES_ON);
    asm volatile("mret;");
}

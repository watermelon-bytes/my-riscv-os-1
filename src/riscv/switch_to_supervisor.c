#include <klibc/k_assert.h>
#include <klibc/printf.h>
#include <riscv/csr_operations.h>
#include <riscv/interrupts/interrupt_sources.h>
#include <riscv/interrupts/interrupts.h>
#include <riscv/pmp/pmpcfg.h>
#include <riscv/switch_to_supervisor.h>
extern void s_mode_handle();

void riscv_switch_to_supervisor(void* jump_addr) {
#if __riscv_xlen == 32
    union mstatus_32 mstatus = {.raw_value_ = READ_CSR(mstatus)};
#elif __riscv_xlen == 64
    union mstatus_64 mstatus = {.lo = {.raw_value_ = READ_CSR(mstatus)}};
#else
    #error "unknown / unsupported __riscv_xlen"
#endif

    // mret will switch privilege level to value of mstatus.MPP
    mstatus.machine_prev_privilege = RISCV_PRIV_LVL_SUPERVISOR;
    mstatus.machine_prev_interrupt_enable = 0;
    mstatus.machine_interrupt_enable = 0;

#if __riscv_xlen == 32
    WRITE_CSR(pmpaddr0, WORD_MAX);
    WRITE_CSR(pmpaddr1, WORD_MAX);
#elif __riscv_xlen == 64
    WRITE_CSR(pmpaddr0, WORD_MAX);
#else
    #error "incorrect or unsupported __riscv_xlen"
#endif
    WRITE_CSR(pmpcfg0, 0x0f);
    // WRITE_CSR(pmpcfg0, (RISCV_PMPCFG_EXECUTABLE | RISCV_PMPCFG_LOCKED |
    //           RISCV_PMPCFG_READABLE | RISCV_PMPCFG_WRITABLE |
    //           RISCV_PMPCFG_ADDR_MODE_TOP));
    WRITE_CSR(satp, 0);
    asm volatile("sfence.vma x0, x0;");

    union riscv_trap_vector new_stvec = {.base = &s_mode_handle};
    WRITE_CSR(stvec, new_stvec.raw_value);

    WRITE_CSR(mstatus, mstatus.raw_value_);
    WRITE_CSR(mepc, (uintptr_t)jump_addr);
    WRITE_CSR(medeleg, RISCV_ALL_INTR_SOURCES_ON);
    WRITE_CSR(mideleg, RISCV_ALL_INTR_SOURCES_ON);
    asm volatile("mret;");
    __builtin_unreachable();
}

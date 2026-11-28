#include <klibc/k_assert.h>
#include <klibc/printf.h>
#include <klibc/utils.h>
#include <riscv/csr_operations.h>
#include <riscv/timer.h>

void stimer_sleep(const uint cycles) {
    u32 stime_lo = READ_CSR(time);
    u32 stime_hi = READ_CSR(timeh);
    printf("time = 0x%x\n", stime_lo);
    printf("timeh = 0x%x\n", stime_hi);
    const u32 new_stimecmp_lo = stime_lo + cycles;
    if (new_stimecmp_lo < stime_lo) {
        stime_hi++;
        WRITE_CSR(stimecmph, stime_hi);
        DEBUG_ASSERT(READ_CSR(stimecmph) == stime_hi);
    }
    WRITE_CSR(stimecmp, new_stimecmp_lo);
    DEBUG_ASSERT(READ_CSR(stimecmp) == new_stimecmp_lo);
}

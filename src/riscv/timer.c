#include <klibc/utils.h>
#include <riscv/timer.h>

// Sets time comparator to current time and adds the speicified in the first
// parameter count of milliseconds
SET_OPTIMIZATION_LVL(3)
void timer_sleep(uint cycles) {
#if __riscv_xlen == 32
    // Assuming little endian
    volatile u32* mtime = (volatile u32*)present_timer.mtime;
    volatile u32* mtimecmp = (volatile u32*)present_timer.mtimecmp;

    u32 lower_mtime = mtime[0];
    u32 higher_mtime = mtime[1];
    // Check for overflow
    const u32 new_mtimecmp_lo = lower_mtime + cycles;
    if (new_mtimecmp_lo < lower_mtime) {
        higher_mtime++;
    }
    mtimecmp[0] = new_mtimecmp_lo;
    mtimecmp[1] = higher_mtime;
#elif __riscv_xlen == 64
    u64 mtime = *(present_timer.mtime);
    *present_timer.mtimecmp = mtime + cycles;
#endif
}

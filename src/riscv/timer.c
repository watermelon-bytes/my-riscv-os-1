#include <libfdt.h>
#include <riscv/timer.h>

// Sets time comparator to current time and adds the speicified in the first
// parameter count of milliseconds
void timer_sleep(uint cycles) {
#if __riscv_xlen == 32
    // Assuming little endian
    const u32* const mtime = (void*)present_timer.mtime;
    u32* const mtimecmp = (void*)present_timer.mtimecmp;

    u32 lower_mtime = mtime[1];
    u32 higher_mtime = mtime[0];
    // Check for overflow
    const u32 new_mtimecmp_lo = lower_mtime + cycles;
    if (new_mtimecmp_lo < lower_mtime) {
        higher_mtime++;
    }
    mtimecmp[1] = new_mtimecmp_lo;
    mtimecmp[0] = higher_mtime;
    return;
#elif __riscv_xlen == 64
    u64 mtime = *(present_timer.mtime);
    *present_timer.mtimecmp = mtime + cycles;
#endif
}

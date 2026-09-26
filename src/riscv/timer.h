#pragma once
#include <klibc/types.h>

extern struct riscv_timer {
    reg_t* mtime;
    reg_t* mtimecmp;
} present_timer;

// Sets time comparator to current time and adds the speicified in the first
// parameter count of milliseconds
void timer_sleep(const uint cycles);

#pragma once
#include <types.h>

struct riscv_timer {
    u64* mtime;
    u64* mtimecmp;
};

extern struct riscv_timer present_timer;
void timer_sleep(uint cycles);

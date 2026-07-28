#pragma once
#include <types.h>

struct riscv_timer {
    // Assuming little endian
    enum { HIGHER_32BITS_OF_64, LOWER_32BITS_OF_64 };
    u64* mtime;
    u64* mtimecmp;
};

extern struct riscv_timer present_timer;
void timer_sleep(uint cycles);

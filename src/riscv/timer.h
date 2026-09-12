#pragma once
#include <types.h>

struct riscv_timer {
    reg_t* mtime;
    reg_t* mtimecmp;
};

extern struct riscv_timer present_timer;
void timer_sleep(uint cycles);

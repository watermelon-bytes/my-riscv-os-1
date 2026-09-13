#pragma once
#include <klibc/types.h>

extern struct riscv_timer {
    reg_t* mtime;
    reg_t* mtimecmp;
} present_timer;

void timer_sleep(uint cycles);

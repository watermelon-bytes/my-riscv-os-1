#pragma once
#include <types.h>

struct riscv_timer {
    // Assuming little endian
    enum { HIGHER_32BITS_OF_64, LOWER_32BITS_OF_64 };
    u32 (*mtime_addr)[2];
    u32 (*mtimecmp_addr)[2];
};

int discover_timer(const void* device_tree);

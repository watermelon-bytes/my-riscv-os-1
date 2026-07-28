#pragma once
#include <riscv/interrupts.h>

int discover_clint_from_dtb(const void* tree);

struct riscv_timer clint_init_timer();

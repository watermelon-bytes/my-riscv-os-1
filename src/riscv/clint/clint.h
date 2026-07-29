#pragma once
#include <riscv/interrupts/interrupts.h>

int discover_clint(const void* tree);

struct riscv_timer clint_init_timer();

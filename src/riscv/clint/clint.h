#pragma once
#include <riscv/interrupts.h>

int discover_clint_from_dtb(const void* tree);

void clint_init_timer();

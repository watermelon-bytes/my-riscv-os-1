#pragma once
#include <riscv/interrupts/interrupts.h>

/*
 * @brief Tries to fetch a CLINT node in the given Device Tree
 * @return
 * -1 if the CLINT node is not present
 * -2 if the CLINT node was found but could not be parsed
 * 0 if CLINT device registers were discovered successfully
 *
 */
int discover_clint(const void* device_tree);

/*
 * @brief Fills a structure `riscv_timer`,  based on the CLINT base address,
 * and returns it.
 */
struct riscv_timer clint_init_timer();

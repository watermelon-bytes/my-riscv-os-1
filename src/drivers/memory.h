#pragma once
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_MEM_REGIONS 16

_Static_assert(
    sizeof(void*) == sizeof(uintptr_t),
    "Impossible error: sizeof(uintptr_t) != sizeof(void*)");  // who knows

int register_memory_regions(const uint32_t* first_cell, size_t len);

enum { SUCCESS, TOO_MANY_REGIONS };

int extract_ram_region_info(const void* device_tree, int node_offset);

#ifdef DEBUG
void log_detected_memory();
#endif

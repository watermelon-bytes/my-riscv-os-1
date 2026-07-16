#pragma once
#include <klibc/types.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_MEM_REGIONS 16

extern struct space {
    uintptr_t physicaddr;
    size_t space_size;
} ram_regions[MAX_MEM_REGIONS];
extern uint ram_regions_index;

int register_memory_regions(const uint32_t* first_cell, size_t len);

int extract_ram_region_info(const void* device_tree, int node_offset);

i32 get_total_mem();

#ifndef NDEBUG
void log_detected_memory();
#endif

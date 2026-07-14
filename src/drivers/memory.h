#pragma once
#include <types.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_MEM_REGIONS 16

int register_memory_regions(const uint32_t* first_cell, size_t len);

//
int extract_ram_region_info(const void* device_tree, int node_offset);

i32 get_total_mem();

#ifndef NDEBUG
void log_detected_memory();
#endif

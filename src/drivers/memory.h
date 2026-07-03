#pragma once
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_MEM_REGIONS 16

struct {
    uintptr_t beginning_physicaddr;
    size_t space_size;
} ram_regions[MAX_MEM_REGIONS] = {};

/* TODO: Mark as __init and free up later (impossible currently because paging
 * is not implemented) */
bool set_ram_bounds(uintptr_t begin, uintptr_t end);

_Static_assert(
    sizeof(void*) == sizeof(uintptr_t),
    "Impossible error: sizeof(uintptr_t) != sizeof(void*)");  // who knows

bool register_memory_regions(const uint32_t* first_cell, size_t len);

enum { SUCCESS, REGIONS_OVERLAP, TOO_MANY_REGIONS };

int extract_ram_region_info(const void* device_tree, int node_offset);

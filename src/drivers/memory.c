#include <libfdt.h>
#include <init_devices.h>
#include <types.h>
#include <drivers/memory.h>

// TODO: replace raw `int` with `enum` or something
int extract_ram_region_info(const void* device_tree, int node_offset) {
    int length;
    // properties are constructed using 32-bit "cells"
    const uint32_t* ptr = fdt_getprop(device_tree, node_offset, "reg", &length);
    if (!ptr) {
        return length;
    }
    return register_memory_regions(ptr, length);
}

int register_memory_regions(const uint32_t* first_cell, size_t len) {
    if (first_cell == NULL) return false;
    static size_t index = 0;
    for (; len > 0; --len) {
        if (index >= MAX_MEM_REGIONS) {
            return 1;
        }
        const __auto_type buf = &ram_regions[index++];
        first_cell =
            fetch_native_pointer(&(buf->beginning_physicaddr), first_cell);
        first_cell = fetch_size_field(&(buf->space_size), first_cell);
    }
    return 0;
}

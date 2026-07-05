#include <libfdt.h>
#include <klibc/printf.h>
#include <init_devices.h>
#include <types.h>
#include <drivers/memory.h>

struct space {
    uintptr_t beginning_physicaddr;
    size_t space_size;
} ram_regions[MAX_MEM_REGIONS] = {};

// TODO: replace raw `int` with `enum` or something
int extract_ram_region_info(const void* device_tree, int node_offset) {
    int length;
    const uint32_t* ptr = fdt_getprop(device_tree, node_offset, "reg", &length);
    if (!ptr) {
        return length;
    }

    return register_memory_regions(ptr, length);
}

int register_memory_regions(const uint32_t* cell, size_t len) {
    if (cell == NULL) return false;
    static size_t index = 0;
    const uintptr_t end = (uintptr_t)cell + len;
    while ((uintptr_t)cell < end) {
        if (index >= MAX_MEM_REGIONS) {
            return 1;
        }
        const __auto_type buf = &ram_regions[index++];
        // First comes address, then size
        cell = fetch_native_pointer(&(buf->beginning_physicaddr), cell);
        cell = fetch_size_field(&(buf->space_size), cell);
    }
    return 0;
}

#ifdef DEBUG
void log_detected_memory() {
    for (uint32_t i = 0;
         i < MAX_MEM_REGIONS && ram_regions[i].beginning_physicaddr != 0; ++i) {
        printf("Detected RAM: region %i starts at 0x%x, size = %lu B\n", i + 1,
               ram_regions[i].beginning_physicaddr, ram_regions[i].space_size);
    }
}
#endif

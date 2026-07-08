#include <libfdt.h>
#include <klibc/printf.h>
#include <init_devices.h>
#include <types.h>
#include <drivers/memory.h>

int detect_memory(void* tree) {
    const char memory_str[] = "memory";
    const char prop_len = countof(memory_str);
    int mem_node_offset = fdt_node_offset_by_prop_value(tree, -1, "device_type",
                                                        memory_str, prop_len);
    // Iterate through all nodes with "device_type"="memory"
    while (mem_node_offset >= 0) {
        extract_ram_region_info(tree, mem_node_offset);

        mem_node_offset = fdt_node_offset_by_prop_value(
            tree, mem_node_offset, "device_type", memory_str, prop_len);
    }
#ifdef DEBUG
    printf("-------------------\n");
    log_detected_memory();
    printf("-------------------\n");
#endif
    return 0;
}

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
    const uint32_t* end = (uint32_t*)((uintptr_t)cell + len);
    while (cell < end) {
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
    printf("detected RAM:\n");
    for (uint32_t i = 0;
         i < MAX_MEM_REGIONS && ram_regions[i].beginning_physicaddr != 0; ++i) {
        printf("[region %i] start = at 0x%x, size = %lu B\n", i + 1,
               ram_regions[i].beginning_physicaddr, ram_regions[i].space_size);
    }
}
#endif

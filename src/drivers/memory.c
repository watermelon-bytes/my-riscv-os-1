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
        printf("parsing meta information on physical memory layout...\n");
        if (extract_ram_region_info(tree, mem_node_offset) == -1) {
            return -1;
        }
        mem_node_offset = fdt_node_offset_by_prop_value(
            tree, mem_node_offset, "device_type", memory_str, prop_len);
    }
#ifndef NDEBUG
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
static int ram_regions_index = 0;

// TODO: replace raw `int` with `enum` or something
int extract_ram_region_info(const void* device_tree, int node_offset) {
    if (ram_regions_index >= MAX_MEM_REGIONS) {
        return -1;
    }
    __auto_type buf = &ram_regions[ram_regions_index++];
    return parse_reg(device_tree, node_offset, &buf->beginning_physicaddr,
                     &buf->space_size);
}

#ifndef NDEBUG
void log_detected_memory() {
    printf("detected RAM:\n");
    for (uint32_t i = 0;
         i < MAX_MEM_REGIONS && ram_regions[i].beginning_physicaddr != 0; ++i) {
        printf("[region %i] start = at 0x%x, size = %lu B\n", i + 1,
               ram_regions[i].beginning_physicaddr, ram_regions[i].space_size);
    }
}
#endif

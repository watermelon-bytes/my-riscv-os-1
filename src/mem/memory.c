#include "memory.h"

#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <libfdt.h>
#include <types.h>
#include <utils.h>

int detect_memory(const void* tree) {
    const char memory_str[] = "memory";
    const int prop_len = countof(memory_str);
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
    log_detected_memory();
#endif
    return 0;
}

static struct space {
    uintptr_t physicaddr;
    size_t space_size;
} ram_regions_[MAX_MEM_REGIONS] = {};
static uint ram_regions_index_ = 0;

// TODO: replace raw `int` with `enum` or something
int extract_ram_region_info(const void* device_tree, int node_offset) {
    if (ram_regions_index_ >= MAX_MEM_REGIONS) {
        return -1;
    }
    __auto_type buf = &ram_regions_[ram_regions_index_++];
    const __auto_type res =
        parse_reg(device_tree, node_offset, &buf->physicaddr, &buf->space_size);
    ASSERT(buf->space_size != 0);
    return res;
}

void log_detected_memory() {
#ifndef NDEBUG
    const char breaker[] = "===========================\n";
    printf(breaker);
    printf("detected RAM:\n");
    for (uint32_t i = 0; i < ram_regions_index_; ++i) {
        printf("[region %i] start = at 0x%x, size = %lu B\n", i + 1,
               ram_regions_[i].physicaddr, ram_regions_[i].space_size);
    }
    printf(breaker);
#endif
}

i32 get_total_mem() {
    uint counter = 0;
    for (uint i = 0; i < ram_regions_index_; ++i) {
        counter += ram_regions_[i].space_size;
    }
    return counter;
}

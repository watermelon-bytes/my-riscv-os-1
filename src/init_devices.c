#include <drivers/uart.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <libfdt.h>
#include <mem/memory.h>
#include <utils.h>

_Bool check_device_tree(const void* devtree) {
    if (devtree == NULL) return false;

    const int devicetree_passed = fdt_check_header(devtree);

    if (devicetree_passed != 0) {
        printf("[FATAL] No device tree was passed: %s \n",
               fdt_strerror(devicetree_passed));
        return false;
    }

    const int err = fdt_check_full(devtree, fdt_totalsize(devtree));
    if (err) {
        printf("[FATAL] device tree turned out to be invalid\n");
        return false;
    }
    printf("[OK] Device tree verified\n");
    return true;
}

// @param cell_ptr: Pointer to the start of data location
// @param cells_count: How many 32-bit pieces encode the value
// @return Value represented in cell_ptr[cells_count], if it fits into machine
// register, or UINTPTR_MAX otherwise
SET_OPTIMIZATION_LVL(2)
static uintptr_t fetch_lowest_(const u32* cell_ptr, uint cells_count) {
    const int register_width = sizeof(uintptr_t);
    if (register_width >= cells_count * sizeof(u32)) {
        return *cell_ptr;
    }
    // Loop until
    // cell_ptr == cell_ptr + (size of cell * cells_count) - sizeof(uintptr_t)
    for (; cells_count > register_width / sizeof(u32); --cells_count) {
        if (*cell_ptr != 0) {
            return WORD_MAX;
        }
        cell_ptr++;
    }
    return fdt32_to_cpu(*(uintptr_t*)cell_ptr);
}

// TODO: Now returns CALL_AGAIN if there are more than one pair of address and
// size, but doesn't handle the case where caller needs the next values (returns
// just the same)
enum parsing_error parse_reg(const void* tree, const int node,
                             uintptr_t* begin_addr_buf, size_t* size_buf) {
    int len;
    const u32* reg = fdt_getprop(tree, node, "reg", &len);
    if (len < 0) return len;
    // get #size-cells and #address-cells from parent node
    const int parent = RETURN_IF_LESS_THAN_ZERO(fdt_parent_offset(tree, node));

    uint address_cells =
        RETURN_IF_LESS_THAN_ZERO(fdt_address_cells(tree, parent));
    *begin_addr_buf = fetch_lowest_(reg, address_cells);
    reg += address_cells;

    // size
    const uint size_cells =
        RETURN_IF_LESS_THAN_ZERO(fdt_size_cells(tree, parent));
    *size_buf = fetch_lowest_(reg, size_cells);

    /* Since reg field may contain more than one entry, but they all can't be
     * handled in a single call because the function knows only one pair of
     * buffers and can't know where next buffer is placed, notify the caller if
     * other entries left unparsed yet */
    if ((unsigned)len > sizeof(u32) * (address_cells + size_cells)) {
        return CALL_AGAIN;
    }

    return SUCCESS;
}

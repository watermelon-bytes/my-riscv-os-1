#include <drivers/uart.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <libfdt.h>
#include <mem/memory.h>
#include <utils.h>

static bool configured = false;
_Bool check_device_tree(const void* devtree) {
    if (devtree == NULL) return false;

    const int devicetree_passed = fdt_check_header(devtree);

    if (devicetree_passed != 0) {
        printf("FATAL: no device tree was passed: %s \n",
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

// assumes the device tree is valid
int init_devices(void* fdt) {
    const int root = fdt_path_offset(fdt, "/");
    printf("device tree root found\n");

    if (root < 0) {
        printf("[FATAL] can't find device tree root; errors occured: %s \n",
               fdt_strerror(-root));
        return -1;
    }

    RETURN_ON_ERR(configure_field_size(fdt, root));

    for (int node = fdt_first_subnode(fdt, root); node >= 0;
         node = fdt_next_subnode(fdt, node)) {
        int err;
        const char* name = fdt_get_name(fdt, node, &err);
        if (err < 0) {
            printf(fdt_strerror(err));
            continue;
        }

        printf("/%s - ", name);
        device_initializer_func initializer = get_device_initializer(name);
        if (initializer != NULL) {
            printf("calling '%s' initializer...\n", name);
            initializer(fdt, node);
        } else {
            printf("no valid initializer for device: '%s'\n", name);
        }
    }
    return 0;
}

// Could have used enum here ??
const char* possible_names[] = {"memory", "poweroff", "platform-bus", "flash",
                                "soc"};
device_initializer_func matching_initializers[countof(possible_names)] = {
    extract_ram_region_info,
};
// TODO: add initializers for more possible nodes

// Defines which device initializer to call and performs the call
device_initializer_func get_device_initializer(const char* name) {
    for (uint32_t i = 0; i < sizeof(possible_names) / sizeof(possible_names[0]);
         ++i) {
        if (strncmp(name, possible_names[i], strlen(possible_names[i])) == 0) {
            return matching_initializers[i];
        }
    }
    return NULL;
}

const int register_width = sizeof(uintptr_t);
static int cells_to_represent_pointer = 0;
static int cells_to_represent_size = 0;

int get_sizeof_one_descriptor() {
    return cells_to_represent_pointer + cells_to_represent_size;
}

int configure_field_size(void* fdt, int root) {
    if (configured) return 1;
    cells_to_represent_size = fdt_size_cells(fdt, root);
    if (cells_to_represent_size < 0) return cells_to_represent_size;

    cells_to_represent_pointer = fdt_address_cells(fdt, root);
    if (cells_to_represent_pointer < 0) return cells_to_represent_pointer;

    configured = true;
    return 0;
}

// @param cell_ptr: Pointer to the start of data location
// @param cells_count: How many 32-bit pieces encode the value
// @return Value represented in cell_ptr[cells_count], if it fits into machine
// register, or UINTPTR_MAX otherwise
SET_OPTIMIZATION_LVL(2)
static uintptr_t fetch_lowest_(const u32* cell_ptr, uint cells_count) {
    if (register_width >= cells_count * sizeof(u32)) {
        return *cell_ptr;
    }
    // Loop until
    // cell_ptr == cell_ptr + (size of cell * cells_count) - sizeof(uintptr_t)
    for (; cells_count > register_width / sizeof(u32); --cells_count) {
        if (*cell_ptr != 0) {
            return UINTPTR_MAX;
        }
        cell_ptr++;
    }
    return fdt32_to_cpu(*(uintptr_t*)cell_ptr);
}

// Supports only one address-size pair in reg
/* TODO: Return some specific value like CALL_AGAIN to notify the caller that
 * <reg> field contains more than one entry but we can't handle it because we
 * can't know where we have to store result, and add argument for caller to
 * index the necessary entry or store an iterator in static variable */
int parse_reg(const void* tree, const int node, uintptr_t* begin_addr_buf,
              size_t* size_buf) {
    if (!configured) configure_field_size(tree, fdt_path_offset(tree, "/"));
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

    return 0;
}

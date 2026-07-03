#include <init_devices.h>
#include <types.h>
#include <drivers/uart.h>
#include <libfdt.h>
#include <drivers/memory.h>

_Bool check_device_tree(const void* devtree) {
    uart_println("[in function check_device_tree]");
    if (devtree == NULL) return false;

    const int devicetree_passed = fdt_check_header(devtree);

    if (devicetree_passed != 0) {
        uart_println("FATAL: device tree not passed");
        uart_println(fdt_strerror(devicetree_passed));
        return false;
    }
    uart_println("detected possibly valid device tree!");

    const int err = fdt_check_full(devtree, fdt_totalsize(devtree));
    if (err) {
        uart_println("[FATAL] device tree turned out to be invalid");
        return false;
    }
    uart_println("[OK] device tree has been verified");
    return true;
}

// assumes the device tree is valid
int init_devices(void* fdt) {
    const int root = fdt_path_offset(fdt, "/");
    uart_println("device tree root found");

    if (root < 0) {
        uart_println("[FATAL] can't find device tree root; errors occured:");
        uart_println(fdt_strerror(-root));
        return -1;
    }
    for (int node = fdt_first_subnode(fdt, root); node >= 0;
         node = fdt_next_subnode(fdt, node)) {
        int err;
        const char* name = fdt_get_name(fdt, node, &err);
        if (err < 0) {
            uart_println(fdt_strerror(err));
            continue;
        }

        putchar('/');
        uart_println(name);
        device_initializer_func initializer = get_device_initializer(name);
        if (initializer != NULL) {
            initializer(fdt, node);
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
        if (strcmp(name, possible_names[i]) == 0) {
            return matching_initializers[i];
        }
    }
    return NULL;
}

static int cells_to_represent_pointer = 0;
static int cells_to_represent_size = 0;
static bool configured = false;
int configure_field_size(void* fdt, int root) {
    if (configured) return 1;
    cells_to_represent_size = fdt_size_cells(fdt, root);
    if (cells_to_represent_size < 0) return cells_to_represent_size;

    cells_to_represent_pointer = fdt_address_cells(fdt, root);
    if (cells_to_represent_pointer < 0) return cells_to_represent_pointer;

    configured = true;
    return 0;
}

// Could just return address and 0xFFFFFFF... on fault?
// But let's stop overthinking
bool fetch_native_pointer(uintptr_t* result, const u32* cells) {
    if (native_pointer_size >= cells_to_represent_pointer * sizeof(u32)) {
        // NOTE: we assume that native pointer size can't be less than 32 bit
        // width
        *result = *cells;
        return true;
    }
    const u32* p;
    for (p = cells; p < cells + cells_to_represent_pointer - 1; p++) {
        if (*p != 0) {
            return false;
        }
    }
    // Assuming little endian
    *result = __builtin_bswap32(*p);
    return true;
}

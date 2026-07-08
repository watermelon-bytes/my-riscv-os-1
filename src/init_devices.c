#include <init_devices.h>
#include <klibc/printf.h>
#include <utils.h>
#include <drivers/uart.h>
#include <libfdt.h>
#include <drivers/memory.h>

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
    printf("[OK] device tree has been verified\n");
    return true;
}

#define RETURN_ON_ERR(function_call)       \
    ({                                     \
        __auto_type _temp = function_call; \
        if (_temp) {                       \
            return _temp;                  \
        }                                  \
    })

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
    uart_println("configure_field_size called");
    if (configured) return 1;
    cells_to_represent_size = fdt_size_cells(fdt, root);
    if (cells_to_represent_size < 0) return cells_to_represent_size;

    cells_to_represent_pointer = fdt_address_cells(fdt, root);
    if (cells_to_represent_pointer < 0) return cells_to_represent_pointer;

    configured = true;
    return 0;
}

// returns pointer to next cell
static const u32*  //
fetch_lowest(const u32* cells, uint cells_count, uintptr_t* buff) {
    if (register_width >= cells_count * sizeof(u32)) {
        // NOTE: assuming native pointer size can't be less than 32 bit width
        *buff = fdt32_to_cpu(*cells);
        return ++cells;
    }

    const u32* p;
    for (p = cells; p < cells + cells_count - 1; p++) {
        if (*p != 0) {
            // printf()
            *buff = UINTPTR_MAX;
            goto exit;
        }
    }
    // Assuming little endian
    *buff = fdt32_to_cpu(*p);
exit:
    return ++p;
}

const u32* fetch_native_pointer(uintptr_t* const result, const u32* cells) {
    return fetch_lowest(cells, cells_to_represent_pointer, result);
}

const u32* fetch_size_field(register_t* result, const u32* cells) {
    return fetch_lowest(cells, cells_to_represent_size, result);
}

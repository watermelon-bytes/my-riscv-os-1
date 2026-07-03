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
        if (err < 0)
            uart_println(fdt_strerror(err));
        else {
            putchar('/');
            uart_println(name);
        }
    }
    return 0;
}

static const struct {
    const char* name;
    // function that accepts node of a device tree
    int (*init_device)(const void* tree, int node);
} possible_names[] = {{"memory", &extract_ram_region_info},
                      {"poweroff"},
                      {"platform-bus"},
                      {"flash"},
                      {"soc"}};
// TODO: add initialisers for more possible nodes

// Defines which device initialiser to call and performs the call
int parse_device_info(const void* tree, int node) {
    const char* name;
    {
        int err;
        name = fdt_get_name(tree, node, &err);
        if (err) return 1;
    }
    int index = -1;
    for (uint32_t i = 0; i < sizeof(possible_names) / sizeof(possible_names[0]);
         ++i) {
        if (strcmp(name, possible_names[i].name) == 0) {
            index = i;
            break;
        }
    }
    if (index < 0 || possible_names[index].init_device == NULL) {
        return 2;
    }

    return possible_names[index].init_device(tree, node);
}

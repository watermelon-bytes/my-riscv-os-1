#include <drivers/uart.h>
#include <libfdt.h>

_Bool check_device_tree(const void* devtree) {
    uart_println("[in function check_device_tree]");
    if (devtree == NULL) return false;
    // 1. Check header

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
    uart_println("function `init_devices` called");
    const int root = fdt_path_offset(fdt, "/");
    uart_println("device tree root found");

    if (root < 0) {
        uart_println("[FATAL] can't find device tree root; errors occured:");
        uart_println(fdt_strerror(-root));
        return -1;
    }
    uart_println("got root");
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

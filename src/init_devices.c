#include <libfdt.h>
#include <uart.h>

int init_devices(void* fdt) {
    uart_println("function `init_devices` called");
    const int root = fdt_path_offset(fdt, "/");
    if (root < 0) {
        uart_println("[FATAL] can't find device tree root; errors occured:");
        uart_println(fdt_strerror(-root));
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

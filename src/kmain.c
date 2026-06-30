#include <init_devices.h>
#include <libfdt.h>
#include <uart.h>

void kmain(int hardt_id, void* devicetree) {
    uart_println("hello!");
    const int devicetree_passed = fdt_check_header(devicetree);
    if (devicetree_passed != 0) {
        uart_println("FATAL: device tree not passed");
        uart_println(fdt_strerror(devicetree_passed));
        __asm__ __volatile__("wfi; \n");
    }

    uart_println("detected device tree");
    init_devices(devicetree);
    uart_println("here");
    __asm__ __volatile__("wfi; \n");
}

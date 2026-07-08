#include <init_devices.h>
#include <klibc/printf.h>
#include <libfdt.h>
#include <drivers/uart.h>
#include <drivers/memory.h>

void halt() {
loop:
    __asm__ __volatile__("wfi; \n");
    goto loop;
}

void kmain(int hardt_id, void* device_tree) {
    printf("Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        printf("halting\n");
        halt();
    }
    configure_field_size(device_tree, fdt_path_offset(device_tree, "/"));
    detect_memory(device_tree);
    halt();
}

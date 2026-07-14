#include <init_devices.h>
#include <klibc/printf.h>
#include <klibc/panic.h>
#include <libfdt.h>
#include <drivers/uart.h>
#include <drivers/memory.h>

void kmain(int hardt_id, void* device_tree) {
    printf("Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        k_panic("invalid device tree was passed");
    }
    detect_memory(device_tree);
    halt();
}

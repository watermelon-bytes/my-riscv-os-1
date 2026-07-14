#include <drivers/memory.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>

void kmain(int hardt_id, void* device_tree) {
    printf("Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        k_panic("invalid device tree was passed");
    }
    detect_memory(device_tree);
    if (get_total_mem() < 4096) {
        k_panic("not enough memory; at least 4KiB required");
    }
    halt();
}

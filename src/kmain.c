#include <drivers/memory.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <riscv/extension_check.h>

void kmain(int hardt_id, void* device_tree) {
    ensure_extensions_present();
    printf("Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        KERNEL_PANIC("invalid device tree was passed");
    }
    detect_memory(device_tree);
    if (get_total_mem() < 4096) {
        KERNEL_PANIC("not enough memory; at least 4KiB required");
    }
    halt();
}

#include <drivers/interrupt_controller.h>
#include <drivers/uart.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <mem/memory.h>
#include <riscv/extension_check.h>

void kmain(int hardt_id, void* device_tree) {
    disable_interrupts();
    // Will already panic if any extension missing
    ensure_extensions_present(RISCV_EXT_MUL_DIV | RISCV_EXT_USER_MODE);
    printf("[OK] Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        KERNEL_PANIC("invalid device tree was passed");
    }
    detect_memory(device_tree);
    init_uart(device_tree);
    /* WARNING: printf is called multiple times before init_uart. Works in QEMU,
     * but if UART isn't placed at 0x10000000, then debug messages will simply
     * waste CPU cycles. Maybe add a preprocessor directive to indicate whether
     * or not target machine is QEMU virt emulator? */
    halt();
}

#include <drivers/uart.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <mem/memory.h>
#include <mem/phys_alloc.h>
#include <riscv/csr_operations.h>
#include <riscv/extension_check.h>
#include <riscv/interrupts/interrupts.h>
#include <riscv/timer.h>

#if defined(__riscv__zbb)
    #define RISCV_OPTIONAL_ZBB RISCV_EXT_BIT_MANIPULATION
#else
    #define RISCV_OPTIONAL_ZBB 0
#endif

// This constant should be used to check extensions (consider moving to header
// file?)
const u32 NECESSARY_RISCV_EXTENSIONS =
    RISCV_OPTIONAL_ZBB | RISCV_EXT_USER_MODE | RISCV_EXT_MUL_DIV;

void kmain(int hardt_id, void* device_tree) {
    disable_interrupts();
    printf("++++++++++++++++++++++++++++\n");
    // Will already panic if any extension missing
    ensure_extensions_present(NECESSARY_RISCV_EXTENSIONS);
    setup_interrupt_handler();
    init_interrupt_controller(device_tree);
    printf("[OK] Booting on on hardware thread %i\n", hardt_id);
    if (!check_device_tree(device_tree)) {
        KERNEL_PANIC("Invalid device tree was passed");
    }
    detect_memory(device_tree);
    init_phys_allocator();
    printf("[FINISHED] Nothing to do left: halting\n");
    /* WARNING: printf, which relies on uart_putchar, is called multiple times
     * before init_uart. Works in QEMU because I hardcoded the UART address, but
     * on other platforms, if UART isn't placed at 0x10000000, then debug
     * messages will simply waste CPU cycles and potentially cause undefined
     * behavior. Maybe add a preprocessor directive to indicate whether or not
     * target machine is QEMU virt emulator? */
    halt();
}

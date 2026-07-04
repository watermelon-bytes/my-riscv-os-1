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
    printf("hello!");

    if (!check_device_tree(device_tree)) {
        halt();
    }
    printf(device_tree);
    uart_println("[back in kmain]");
#ifdef DEBUG
    log_detected_memory();
#endif /* ifdef DEBUG */
    halt();
}

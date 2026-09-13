#include "uart.h"

#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <klibc/utils.h>
#include <libfdt.h>
// set QEMU virt's UART address to default
// not marked as const to avoid hardcoding addresses;
// use of UART is made to provide a convenient way of debugging
#ifndef QEMU_VIRT
static volatile u8* const uart_base = (volatile u8*)0x10000000;
#else
static volatile u8* uart_base = (volatile u8*)0x10000000;
#endif

void _putchar(char c) { uart_base[UART_RECEIVER_BUFF] = c; }

void uart_print(const char* str) {
    while (*str != 0) {
        putchar(*str++);
    }
}

void uart_println(const char* str) {
    uart_print(str);
    putchar('\n');
}

int init_uart(const void* fdt) {
#ifndef QEMU_VIRT
    const int uart_node = RETURN_IF_LESS_THAN_ZERO(
        fdt_node_offset_by_compatible(fdt, -1, "ns16550a"));
    size_t size;
    parse_reg(fdt, uart_node, (uintptr_t*)&uart_base, &size);
    printf("[OK] Found UART: starts at 0x%x, size = %i\n", uart_base, size);
    return 0;
#endif /* ifndef QEMU_VIRT */
}

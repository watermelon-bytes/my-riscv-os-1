#include "uart.h"

#include <init_devices.h>
#include <klibc/panic.h>
#include <libfdt.h>
#include <printf.h>
#include <utils.h>
// set QEMU virt's UART address to default
// not marked as const to avoid hardcoding addresses;
// use of UART is made to provide a convenient way of debugging
static volatile unsigned char* uart = (unsigned char*)0x10000000;

void _putchar(char c) { putchar((int)c); }

void putchar(int c) {
    *uart = c;
    return;
}

void uart_print(const char* str) {
    int c;
    while ((c = *str++) != 0) {
        putchar(c);
    }
}

void uart_println(const char* str) {
    uart_print(str);
    putchar('\n');
}

int init_uart(const void* fdt) {
    int uart_node = RETURN_IF_LESS_THAN_ZERO(
        fdt_node_offset_by_compatible(fdt, -1, "ns16550a"));
    size_t size;
    parse_reg(fdt, uart_node, (uintptr_t*)&uart, &size);
    printf("parsed reg field of UART: starts at 0x%x, size = %i\n", uart, size);
    return 0;
}

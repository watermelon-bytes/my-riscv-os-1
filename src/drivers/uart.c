#include "uart.h"
// set QEMU virt's UART address to default
// not marked as const to avoid hardcoding addresses;
// use of UART is made to provide a convenient way of debugging
volatile unsigned char* uart = (unsigned char*)0x10000000;

void _putchar(char c) { putchar((int)c); }

void putchar(int c) {
    *uart = c;
    return;
}

// Copies string to UART output byte-by-byte
void uart_print(const char* str) {
    int c;
    while ((c = *str++) != 0) {
        putchar(c);
    }
}

// Outputs string to UART and adds newline
void uart_println(const char* str) {
    uart_print(str);
    putchar('\n');
}

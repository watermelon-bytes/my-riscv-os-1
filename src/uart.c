#include <uart.h>
// set QEMU virt's UART address to default
volatile unsigned char* uart = (unsigned char*)0x10000000;

void putchar(int c) {
    *uart = c;
    return;
}

void uart_println(const char* str) {
    while (*str != '\0') {
        putchar(*str);
        str++;
    }
    putchar('\n');
}

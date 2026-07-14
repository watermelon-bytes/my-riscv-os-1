#pragma once
// Collects info on UART from device tree; initializes and configures device
// driver to match current platform's specs
// Returns:
// 0 on success,
// 1 if UART node could not be found in device tree,
// < 0 libfdt's error code
int init_uart(const void* fdt);

// Outputs string to UART and adds newline
void uart_println(const char[]);

// Copies string to UART output byte-by-byte
void uart_print(const char*);

// Outputs a single character to UART register
// (only lowest 8 bits of the parameter `c`)
void putchar(int c);

void _putchar(char);

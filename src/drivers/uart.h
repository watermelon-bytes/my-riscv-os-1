#pragma once
void uart_println(const char[]);
void uart_print(const char*);
void putchar(int);
void _putchar(char);
extern volatile unsigned char* uart;

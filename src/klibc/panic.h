#pragma once
#include <klibc/printf.h>

void halt();
#define KERNEL_PANIC(...)            \
    ({                               \
        printf("[PANIC] halting: "); \
        printf(__VA_ARGS__);         \
        _putchar('\n');              \
        halt();                      \
        __builtin_unreachable();     \
    })

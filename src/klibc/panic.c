#include "panic.h"

#include <klibc/printf.h>

void halt() {
loop:
    __asm__ __volatile__("wfi; \n");
    goto loop;
}

#define KERNEL_PANIC(...)            \
    ({                               \
        printf("[PANIC] halting: "); \
        printf(__VA_ARGS__);         \
        _putchar('\n');              \
        halt();                      \
    })

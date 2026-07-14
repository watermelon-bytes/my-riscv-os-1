#include "panic.h"

#include <klibc/printf.h>

void halt() {
loop:
    __asm__ __volatile__("wfi; \n");
    goto loop;
}

#include "panic.h"

void halt() {
loop:
    __asm__ __volatile__("wfi; \n");
    goto loop;
}

void k_panic(const char reason[]) {
    printf("[PANIC] halting: %s\n", reason);
    halt();
}

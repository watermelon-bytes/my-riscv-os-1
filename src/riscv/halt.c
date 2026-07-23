#include <klibc/panic.h>
#include <klibc/printf.h>
#include <riscv/interrupts.h>

void halt() {
    disable_interrupts();
loop:
    __asm__ __volatile__("wfi; \n");
    goto loop;
}

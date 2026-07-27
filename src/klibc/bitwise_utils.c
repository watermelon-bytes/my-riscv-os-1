#include <bitwise_utils.h>
#include <limits.h>
#include <riscv/extension_check.h>
#include <types.h>
#include <utils.h>

#if defined(__riscv_zbb)
__attribute__((naked)) uint count_trailing_zeroes(unsigned long) {
    asm("ctz a0, a0;"
        "ret;");
}
#else
// TODO: Optimize with binary search or something
uint count_trailing_zeroes(unsigned long x) {
    for (int i = 0; i < sizeof(x) * CHAR_BIT; ++i) {
        const uint shift = BITS_COUNT(__typeof__(x)) - 1;
        if (x & (1 << shift)) {
            return i;
        }
        x <<= 1;
    }
    return 32;
}
#endif

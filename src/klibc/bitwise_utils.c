#include <bitwise_utils.h>
#include <limits.h>
#include <riscv/extension_check.h>
#include <types.h>
#include <utils.h>

#if defined(__riscv_zbb)
inline uint count_trailing_zeroes(unsigned long x) {
    uint res;
    asm("ctz a0, a0;" : "=r"(res) : "r"(x));
    return res;
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

u32 byte_swap_32(const u32 original) {
#define BYTE_MASK(byte_no) ({ UCHAR_MAX << (CHAR_BIT * byte_no); })
    return ((original & BYTE_MASK(0)) << 24) |
           ((original & BYTE_MASK(1)) << 8) | ((original & BYTE_MASK(2)) >> 8) |
           ((original & BYTE_MASK(3)) >> 24);
#undef BYTE_MASK
}

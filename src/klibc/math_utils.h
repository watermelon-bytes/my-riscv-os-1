#pragma once
#include <klibc/types.h>

struct div_t {
    long quot;
    long rem;
};

struct div_t div(long a, long b);

u_long udiv_and_ceil(u_long a, u_long b);

/**
 * Performs unsigned integer division by a power of two.
 *
 * Divides `a` by 2^power_of_two and returns both the quotient and
 * remainder.
 * Should be used when you're not sure that compiler will not optimise the `div`
 * and `rem` instructions away in the `div(long, long)` function.
 *
 * @param a The dividend.
 * @param power_of_two The exponent of the power-of-two divisor; i.e., the
 * divisor is 2^power_of_two.
 *
 * WARNING: `power_of_two` must not exceed the number of bits in `u_long`.
 * Values beyond the bit width of `u_long` are invalid.
 */
struct div_t udiv_py_power_of_2(u_long a, uint power_of_two);

// Those are somewhat generic
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) (min((a), (b)) == a ? b : a)

#include <klibc/math_utils.h>
#include <limits.h>

struct div_t div(long a, long b) {
    return (struct div_t){
        .quot = a / b,
        .rem = a % b,
    };
}

u_long udiv_and_ceil(u_long a, u_long b) { return (a / b) + (a % b ? 1 : 0); }

struct div_t udiv_py_power_of_2(u_long a, uint power_of_two) {
    const u_long significant_bits = ULONG_MAX << power_of_two;
    return (struct div_t){
        .quot = a >> power_of_two,
        .rem = a & ~significant_bits,
    };
}

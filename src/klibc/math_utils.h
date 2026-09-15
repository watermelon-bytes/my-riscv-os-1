#pragma once
#include <klibc/types.h>

struct div_t {
    long quot;
    long rem;
};

struct div_t div(long a, long b);

long udiv_and_ceil(u_long a, u_long b);

struct div_t udiv_py_power_of_2(u_long a, uint power_of_two);

// Those are somewhat generic
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) (min((a), (b)) == a ? b : a)

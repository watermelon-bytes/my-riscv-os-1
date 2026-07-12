#pragma once
#if defined(__GNUC__) || defined(__clang__)
// GCC and Clang use function attributes
#define SET_OPTIMIZATION_LVL(lvl) __attribute__((optimize("O" #lvl)))
#else
#define SET_OPTIMIZATION_LVL(lvl)
#endif

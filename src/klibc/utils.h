#pragma once
#include <types.h>

u32 byte_swap_32(const u32 original);
void word_aligned_memset(register_t* p, const register_t num, size_t count);

#define RETURN_VAL_IF_NULL(func_call, val) \
    ({                                     \
        __auto_type tmp = (func_call);     \
        if (tmp == NULL) return (val);     \
        tmp;                               \
    })

#define RETURN_IF_LESS_THAN_ZERO(func_call) \
    ({                                      \
        __auto_type tmp = (func_call);      \
        if (tmp < 0) return tmp;            \
        tmp;                                \
    })

#define RETURN_ON_ERR(function_call)       \
    ({                                     \
        __auto_type _temp = function_call; \
        if (_temp) {                       \
            return _temp;                  \
        }                                  \
    })

#pragma once
#if defined(__GNUC__) || defined(__clang__)

    #ifndef NDEBUG
        #define SET_OPTIMIZATION_LVL(lvl)
    #else
        #define SET_OPTIMIZATION_LVL(lvl) __attribute__((optimize("O" #lvl)))
    #endif

#else
    #define SET_OPTIMIZATION_LVL(lvl)
//
#endif

#define countof(array) (sizeof(array) / sizeof(array[0]))
#define BITS_COUNT(x) (sizeof(x) * CHAR_BIT)
#define WORD_SIZE ((unsigned)__riscv_xlen)
#define WORD_ALIGNED(x) \
    ((register_t*)((uintptr_t)(x) & ~(sizeof(register_t) - 1)))
#define min(a, b) (a < b ? a : b)

#pragma once
#include <types.h>

u32 byte_swap_32(const u32 original);

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
// GCC and Clang use function attributes
#define SET_OPTIMIZATION_LVL(lvl) __attribute__((optimize("O" #lvl)))
#else
#define SET_OPTIMIZATION_LVL(lvl)
#endif

#define countof(array) (sizeof(array) / sizeof(array[0]))

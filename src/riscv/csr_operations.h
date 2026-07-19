#pragma once
#include <types.h>

#if defined(__GNUC__) || defined(__clang__)
    #define READ_CSR(reg)                                    \
        ({                                                   \
            register_t _tmp;                                 \
            __asm__ volatile("csrr %0, " #reg : "=r"(_tmp)); \
            _tmp;                                            \
        })
    #define WRITE_CSR(reg, val)                                \
        ({                                                     \
            register_t _tmp = (val);                           \
            __asm__ volatile("csrw " #reg ", %0" ::"r"(_tmp)); \
        })
#endif

#pragma once
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
    #define READ_CSR(reg)                                    \
        ({                                                   \
            uintptr_t _tmp;                                  \
            __asm__ volatile("csrr %0, " #reg : "=r"(_tmp)); \
            _tmp;                                            \
        })
    #define WRITE_CSR(reg, val)                                \
        ({                                                     \
            const uintptr_t _tmp = (val);                      \
            __asm__ volatile("csrw " #reg ", %0" ::"r"(_tmp)); \
        })
#else
    #error "Compilers other than GCC and Clang are not supported yet"
#endif

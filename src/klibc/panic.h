#pragma once
#include "printf.h"

void halt();
void k_panic(const char reason[]);

#define ASSERT(condition)                                                    \
    ({                                                                       \
        if (!(condition)) {                                                  \
            printf("-----------------------------");                         \
            printf("[TERMINATED] Assertion failed: " #condition              \
                   " evaluates to 'false' \nat %s: in function %s, line %d", \
                   __FILE__, __PRETTY_FUNCTION__, __LINE__);                 \
            halt();                                                          \
        }                                                                    \
    })
#define ASSERT_WITH_MSG(exp, msg) \
    ({                            \
        if (!(exp)) {             \
            k_panic(msg);         \
        }                         \
    })

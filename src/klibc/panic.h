#pragma once

void halt();

#ifndef NDEBUG
    #define ASSERT(condition)                                    \
        ({                                                       \
            if ((condition) == false) {                          \
                printf("-----------------------------------\n"); \
                printf("[HALT] Assertion failed: " #condition    \
                       " evaluates to 'false'\n"                 \
                       "at %s: in function %s, line %d\n",       \
                       __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                halt();                                          \
            }                                                    \
        })
    #define ASSERT_WITH_MSG(exp, ...)      \
        ({                                 \
            if (!(exp)) {                  \
                KERNEL_PANIC(__VA_ARGS__); \
            }                              \
        })
#else

    #define ASSERT(condition) ;
    #define ASSERT_WITH_MSG(exp, msg) ;

#endif

#define KERNEL_PANIC(...)            \
    ({                               \
        printf("[PANIC] halting: "); \
        printf(__VA_ARGS__);         \
        _putchar('\n');              \
        halt();                      \
    })

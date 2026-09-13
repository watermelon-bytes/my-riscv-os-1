#pragma once

extern void halt();
#ifndef NDEBUG
    #define DEBUG_ASSERT(condition)                              \
        ({                                                       \
            if (!(condition)) {                                  \
                printf("-----------------------------------\n"); \
                printf("[HALT] Assertion failed: " #condition    \
                       " evaluates to 'false'\n"                 \
                       "at %s: in function %s, line %d\n",       \
                       __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                halt();                                          \
            }                                                    \
        })

    #define DEBUG_ASSERT_WITH_MSG(condition, ...) \
        ({                                        \
            if (!(condition)) {                   \
                printf(__VA_ARGS__);              \
                halt();                           \
            }                                     \
        })
#else

    #define DEBUG_ASSERT(condition) ;
    #define DEBUG_ASSERT_WITH_MSG(exp, msg, ...) ;

#endif

#pragma once
extern int printf_(const char* format, ...);
extern void halt();
#ifndef NDEBUG
    #define DEBUG_ASSERT(condition)                               \
        ({                                                        \
            if (!(condition)) {                                   \
                printf_("-----------------------------------\n"); \
                printf_("[HALT] Assertion failed: " #condition    \
                        " evaluates to 'false'\n"                 \
                        "at %s: in function %s, line %d\n",       \
                        __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                halt();                                           \
            }                                                     \
        })

    #define DEBUG_ASSERT_WITH_MSG(condition, ...) \
        ({                                        \
            if (!(condition)) {                   \
                printf_(__VA_ARGS__);             \
                halt();                           \
            }                                     \
        })
#else

    #define DEBUG_ASSERT(condition) ;
    #define DEBUG_ASSERT_WITH_MSG(exp, msg, ...) ;

#endif

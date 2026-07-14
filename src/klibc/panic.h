#pragma once

void halt();
void k_panic(const char reason[]);

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
    #define ASSERT_WITH_MSG(exp, msg) \
        ({                            \
            if (!(exp)) {             \
                k_panic(msg);         \
            }                         \
        })
#else

    #define ASSERT(condition) ;
    #define ASSERT_WITH_MSG(exp, msg) ;

#endif

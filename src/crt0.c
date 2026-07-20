#include <klibc/panic.h>
#include <limits.h>
#include <stdint.h>
#include <types.h>
#include <utils.h>

_Static_assert(
    sizeof(void*) == sizeof(uintptr_t),
    "Impossible error: sizeof(uintptr_t) != sizeof(void*)");  // who knows

#ifndef NDEBUG
    #include <klibc/printf.h>
    #define LOG_CALL \
        ({ printf("function %s was called\n", __PRETTY_FUNCTION__); })
#else
    #define LOG_CALL ;
#endif
// relying on __builtin_'s may look like a weakness
// but it also may be a good time saving and code reusing

SET_OPTIMIZATION_LVL(2)
void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
    LOG_CALL;
    return __builtin_memcpy(dest, src, n);
}

SET_OPTIMIZATION_LVL(2)
void* memset(void* s, int c, size_t n) {
    char* ptr = (char*)s;
    while (n-- > 0) {
        *ptr++ = c;
    }
    return s;
}

SET_OPTIMIZATION_LVL(2)
void* memmove(void* dest, const void* src, size_t n) {
    LOG_CALL;
    return __builtin_memmove(dest, src, n);
}

// TODO: write a normal version
SET_OPTIMIZATION_LVL(2)
int memcmp(const void* s1, const void* s2, size_t n) {
    __auto_type p1 = (u8*)s1;
    __auto_type p2 = (u8*)s2;
    for (; n > 0; n--) {
        if (*p1++ != *p2++) {
            return p1 - p2;
        }
    }
    return 0;
}

SET_OPTIMIZATION_LVL(2)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++, s2++;
    }
    return *(u8*)s1 - *(u8*)s2;
}

SET_OPTIMIZATION_LVL(2)
int strncmp(const char* s1, const char* s2, const size_t sz) {
    for (signed n = sz; n > 0; --n) {
        const int diff = *s1 - *s2;
        if (diff != 0) {
            return diff;
        } else if (*s1 == 0 || *s2 == 0) {
            break;
        }
    }
    return 0;
}

// TODO: Optimise naive implementation
SET_OPTIMIZATION_LVL(2)
size_t strlen(const char* s) {
    // return __builtin_strlen(s);
    size_t i = 0;
    while (s[i] != 0) ++i;
    return i;
}

SET_OPTIMIZATION_LVL(2)
size_t strnlen(const char*, size_t);

// TODO: Optimise naive implementation
SET_OPTIMIZATION_LVL(2)
size_t strnlen(const char* s, size_t maxlen) {
    for (uint32_t i = 0; i < maxlen; ++i) {
        if (s[i] == 0) {
            return i;
        }
    }
    return maxlen - 1;
}

// TODO: Optimise naive implementation
SET_OPTIMIZATION_LVL(2)
void* memchr(const void* ptr__, int ch, size_t count) {
    __auto_type ptr = (const u_char*)ptr__;
    while (count > 0) {
        if (*ptr == ch) {
            return (void*)ptr;
        }
        ptr++, count--;
    }
    return NULL;
}

SET_OPTIMIZATION_LVL(2)
char* strrchr(const char* str, int ch) {
    while (*str != 0) {
        if (*str == ch) return (char*)str;
    }
    return NULL;
}

u32 byte_swap_32(const u32 original) {
#define BYTE_MASK(byte_no) ({ UCHAR_MAX << (CHAR_BIT * byte_no); })
    return ((original & BYTE_MASK(0)) << 24) |
           ((original & BYTE_MASK(1)) << 8) | ((original & BYTE_MASK(2)) >> 8) |
           ((original & BYTE_MASK(3)) >> 24);
#undef BYTE_MASK
}

// WARNING: Only use if count is known to be divisible by machine word size (in
// our case, 4)
void word_aligned_memset(void* p, const register_t num, size_t count) {
    ASSERT(count % sizeof(register_t) == 0);
    ASSERT((uintptr_t)p % sizeof(register_t) == 0);
    ASSERT(p != 0x0);
    // probably should leave those asserts because they're debug-only and may
    // help catch something weird
    for (register_t* ptr = p; count > 0; count -= sizeof(register_t)) {
        *ptr++ = num;
    }
}

#undef LOG_CALL

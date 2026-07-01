
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

// relying on __builtin_'s may look like a weakness
// but it also may be a good time saving and code reusing

__attribute__((always_inline)) void* memcpy(void* __restrict dest,
                                            const void* __restrict src,
                                            size_t n) {
    return __builtin_memcpy(dest, src, n);
}

__attribute__((always_inline)) void* memset(void* s, int c, size_t n) {
    return __builtin_memset(s, c, n);
}
__attribute__((always_inline)) void* memmove(void* dest, const void* src,
                                             size_t n) {
    return __builtin_memmove(dest, src, n);
}

__attribute__((always_inline)) int memcmp(const void* s1, const void* s2,
                                          size_t n) {
    return __builtin_memcmp(s1, s2, n);
}

__attribute__((always_inline))  //
int strcmp(const char* s1, const char* s2) {
    return __builtin_strcmp(s1, s2);
}

__attribute__((always_inline)) int strncmp(const char* s1, const char* s2,
                                           size_t n) {
    return __builtin_strncmp(s1, s2, n);
}

// TODO: Optimise naive implementation
__attribute__((always_inline)) size_t strlen(const char* s) {
    // return __builtin_strlen(s);
    size_t i = 0;
    while (s[i] != 0) ++i;
    return i;
}

__attribute__((optimize("O2")))  // should work?
size_t strnlen(const char*, size_t);

// TODO: Optimise naive implementation
size_t strnlen(const char* s, size_t maxlen) {
    for (uint32_t i = 0; i < maxlen; ++i) {
        if (s[i] == 0) {
            return i;
        }
    }
    return maxlen - 1;
}

// size_t strnlen(const char* s, size_t maxlen) {
//     uint32_t buf;
//     uint32_t len = 0;
//     uint32_t current_byte;  // only lowest 8 bits are used
//
//     {
//         unsigned int unaligned = (uintptr_t)s & 0b11;
//         if (!unaligned) {
//             goto further;
//         }
//         for (; unaligned > 0; --unaligned, s++, len++) {
//             if (len > maxlen || *s == '\0') return len;
//         }
//     further:;
//     }
//
//     uint32_t* ptr = (uint32_t*)s;
//     uint32_t remainder = maxlen & 0xFFu;
//     maxlen &= (~0xFFul);
//
//     while (len < maxlen) {
//         buf = *ptr++;
//         if (!buf) goto exit;
//         while (buf != 0) {
//             current_byte = buf & UCHAR_MAX;
//             if (current_byte == 0) {
//                 goto exit;
//             }
//             if (maxlen - len == 1) {
//                 return len;
//             }
//             len++;
//             buf = buf >> CHAR_BIT;
//         }
//     }
//     for (s = (char*)ptr; remainder > 0; s++, remainder--) {
//         const char c = *s;
//         if (c == 0) {
//             return len;
//         }
//         len++;
//     }
// exit:
//     return len;
// }

// TODO: Optimise naive implementation
typedef unsigned char u_char;
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
__attribute__((always_inline)) char* strrchr(const char* str, int ch) {
    return __builtin_strrchr(str, ch);
}

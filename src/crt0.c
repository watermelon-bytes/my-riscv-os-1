#include <limits.h>
#include <stddef.h>
#include <stdint.h>

void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
    return __builtin_memcpy(dest, src, n);
}

void* memset(void* s, int c, size_t n) { return __builtin_memset(s, c, n); }

void* memmove(void* dest, const void* src, size_t n) {
    return __builtin_memmove(dest, src, n);
}

int memcmp(const void* s1, const void* s2, size_t n) {
    return __builtin_memcmp(s1, s2, n);
}

int strcmp(const char* s1, const char* s2) { return __builtin_strcmp(s1, s2); }

int strncmp(const char* s1, const char* s2, size_t n) {
    return __builtin_strncmp(s1, s2, n);
}

size_t strlen(const char* s) { return __builtin_strlen(s); }

__attribute__((optimize("O3")))  // should work?
size_t strnlen(const char*, size_t);

size_t strnlen(const char* s, size_t maxlen) {
    for (uint32_t i = 0; i < maxlen; ++i) {
        if (s[i] == 0) {
            return i;
        }
    }
    return maxlen;
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

void* memchr(const void* ptr, int ch, size_t count) {
    return __builtin_memchr(ptr, ch, count);
}

char* strrchr(const char* str, int ch) { return __builtin_strrchr(str, ch); }

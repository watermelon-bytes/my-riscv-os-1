#pragma once
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t i32;
typedef uint8_t u8;
typedef int8_t i8;
typedef uintptr_t register_t;
typedef uintptr_t reg_t;
typedef uintptr_t word_t;
typedef unsigned char u_char;

#define WORD_MAX UINTPTR_MAX
#define WORD_SIZE (__riscv_xlen)

#pragma once
#include <klibc/types.h>

// /* sstatus (SXLEN bits).  For RV32 this is the whole register;
//  * for RV64 the upper half contains UXL + reserved + SD. */
// union sstatus {
//     uintptr_t raw_value;
//
//     /* Common low 32 bits (identical layout on RV32 and in the low half of
//     RV64)
//      */
//     struct {
//         u32 : 1;       /* WPRI */
//         u32 sie : 1;   /* SIE  - Supervisor interrupt enable        */
//         u32 : 3;       /* WPRI */
//         u32 spie : 1;  /* SPIE – previous SIE                        */
//         u32 ube : 1;   /* UBE  – U-mode endianness                   */
//         u32 : 1;       /* WPRI */
//         u32 spp : 1;   /* SPP  – previous privilege (can't be M)     */
//         u32 vs : 2;    /* VS[1:0] – vector extension state           */
//         u32 : 2;       /* WPRI (was HPP / part of MPP)               */
//         u32 fs : 2;    /* FS[1:0] – floating-point state             */
//         u32 xs : 2;    /* XS[1:0] – user-extension state             */
//         u32 : 1;       /* WPRI (MPRV is M-only)                      */
//         u32 sum : 1;   /* SUM  – permit Supervisor to access User Memory */
//         u32 mxr : 1;   /* MXR  – Make eXecutable Readable            */
//         u32 : 3;       /* WPRI (TVM/TW/TSR are M-only)               */
//         u32 spelp : 1; /* SPELP (Zicfilp)                            */
//         u32 sdt : 1;   /* SDT  (Ssdbltrp)                            */
//         u32 : 6;       /* WPRI */
//         u32 sd : 1;    /* SD   – any of FS/VS/XS dirty (RV32 only)   */
//     } __attribute__((packed));
//
// #if __riscv_xlen == 64
//     struct {
//         u64 : 1;
//         u64 sie : 1;
//         u64 : 3;
//         u64 spie : 1;
//         u64 ube : 1;
//         u64 : 1;
//         u64 spp : 1;
//         u64 vs : 2;
//         u64 : 2;
//         u64 fs : 2;
//         u64 xs : 2;
//         u64 : 1;
//         u64 sum : 1;
//         u64 mxr : 1;
//         u64 : 3;
//         u64 spelp : 1;
//         u64 sdt : 1;
//         u64 : 7;     /* WPRI */
//         u64 uxl : 2; /* UXL[1:0] – effective XLEN in U-mode        */
//         u64 : 29;    /* WPRI */
//         u64 sd : 1;  /* SD */
//     } __attribute__((packed));
// #endif
// };

enum : unsigned {
    /*  */
    RISCV_SSTATUS_SIE = 1u << 1,

    /* Saved previous global interrupt-enable state, i.e. backs up the previous
       value of SIE. */
    RISCV_SSTATUS_SPIE = 1u << 5,

    /*  */
    RISCV_SSTATUS_SPP = 1u << 8,

    /*  */
    RISCV_SSTATUS_VECTOR_EXT_MASK = 0b11u << 9,

    /**/
    RISCV_SSTATUS_FS_EXT_MASK = 0b11u << 13,

    /**/
    RISCV_SSTATUS_XS_EXT_MASK = 0b11u << 15,

    /**/
    RISCV_SSTATUS_SUM = 1u << 18,

    /**/
    RISCV_SSTATUS_MXR = 1u << 19,

    /**/
    RISCV_SSTATUS_SPELP = 1u << 23,

    /**/
    RISCV_SSTATUS_SDT = 1u << 24,

    /**/
    RISCV_SSTATUS_SD = 1u << 31,
};

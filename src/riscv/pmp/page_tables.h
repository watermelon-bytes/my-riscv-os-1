#include <klibc/types.h>

union rv32_page_table_entry {
    u32 raw_value;
    __attribute__((packed)) struct {
        u32 present : 1;
        /**< Denotes whether the entry is valid. */

        u32 readable : 1;
        u32 writable : 1;
        u32 executable : 1;

        u32 user_accessible : 1; /**< When set, the page descriped by this entry
                                    belongs to User code. */

        u32 global : 1;

        u32 accessed : 1;
        u32 dirty : 1;

        // These bits can be used for whatever we want
        u32 reserved_for_kernel : 2;

        u32 physical_page_number_1 : 10;
        u32 physical_page_number_2 : 12;
    };
};

_Static_assert(sizeof(union rv32_page_table_entry) == sizeof(u32),
               "Page table entry in RV32 must be 32 bits wide");

enum riscv_satp_mode {
    /* this enum was copied from:
       https://osdev.wiki/wiki/RISC-V_Paging#A_Note_On_Recursive_Table_Mapping
     */

    // Direct-mapped; virtual memory disabled.
    RISCV_SATP_BARE,

    // Page-based 32-bit virtual memory (only 32-bit)
    RISCV_SATP_SV32,

    // The following VM modes are only available in 64-bit RISC-V

    // RISC-V page-based 39-bit virtual memory.
    RISCV_SATP_SV39 = 8,
    // RISC-V page-based 48-bit virtual memory.
    RISCV_SATP_SV48,
    // RISC-V page-based 57-bit virtual memory.
    RISCV_SATP_SV57,

};

struct satp32 {
    u32 table_root_ppn : 22;
    u32 asid : 9;
    enum riscv_satp_mode mode : 1;
};

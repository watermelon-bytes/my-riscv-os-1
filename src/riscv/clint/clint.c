#include <init_devices.h>
#include <klibc/printf.h>
#include <klibc/utils.h>
#include <libfdt.h>
#include <riscv/csr_operations.h>
#include <riscv/timer.h>

#if __STDC_VERSION__ >= 202311l
constexpr __auto_type CLINE_MSIP_OFFSET = 0x0u;
constexpr __auto_type CLINT_MTIMECMP_OFFSET = 0x4000u;
constexpr __auto_type CLINT_MTIME_OFFSET = 0xBFF8u;
#else
    #define CLINE_MSIP_OFFSET 0x0u
    #define CLINT_MTIMECMP_OFFSET 0x4000u
    #define CLINT_MTIME_OFFSET 0xBFF8u
#endif

static uintptr_t clint_addr;

int discover_clint_from_dtb(const void* tree) {
    int clint_node = fdt_node_offset_by_compatible(tree, -1, "riscv,clint0");
    size_t size;
    if (clint_node > 0) {
        if (parse_reg(tree, clint_node, &clint_addr, &size) != 0) {
            return -2;
        }
    }
    return -1;
}

#if __riscv_xlen == 32
u32 (*clint_get_time())[2] {
    return (u32(*)[2])(clint_addr + CLINT_MTIME_OFFSET);
}

// Returns mtimecmp for the current hart.
u32 (*clint_get_mtimecmp())[2] {
    u32(*const mtimecmp_base)[2] = (void*)(clint_addr + CLINT_MTIMECMP_OFFSET);
    const int hart_id = READ_CSR(mhartid);
    return &mtimecmp_base[hart_id];
}

#else
// TODO: Implement mtime / mtimecmp accesses as if native bus size is 64 bit
#endif

// Since struct riscv_timer fits into 2 registers, we may return it as value
// (will be placed in a0-a1, accoring to the RISC-V calling convention)
struct riscv_timer clint_init_timer() {
    return (struct riscv_timer){.mtime_addr = clint_get_time(),
                                .mtimecmp_addr = clint_get_mtimecmp()};
}

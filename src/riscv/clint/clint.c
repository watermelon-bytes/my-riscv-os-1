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
    #define CLINT_MSIP_OFFSET 0x0u
    #define CLINT_MTIMECMP_OFFSET 0x4000u
    #define CLINT_MTIME_OFFSET 0xBFF8u
#endif

static uintptr_t clint_addr;

int discover_clint(const void* tree) {
    int clint_node = fdt_node_offset_by_compatible(tree, -1, "riscv,clint0");
    size_t size;
    if (clint_node > 0) {
        if (parse_reg(tree, clint_node, &clint_addr, &size) != 0) {
            return -2;
        }
        printf("[OK] CLINT base address = %p\n", clint_addr);
        return 0;
    }
    return -1;
}

static u64* clint_get_time() { return (u64*)(clint_addr + CLINT_MTIME_OFFSET); }

// Returns mtimecmp for the current hart.
static u64* clint_get_mtimecmp() {
    __auto_type mtimecmp_base = (u64*)(clint_addr + CLINT_MTIMECMP_OFFSET);
    return &mtimecmp_base[READ_CSR(mhartid)];
}

struct riscv_timer clint_init_timer() {
    // Requires no stack slots for return value as it fits into a0-a1
    return (struct riscv_timer){.mtime = (reg_t*)clint_get_time(),
                                .mtimecmp = (reg_t*)clint_get_mtimecmp()};
}

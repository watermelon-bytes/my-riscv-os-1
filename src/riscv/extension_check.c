#include <klibc/panic.h>
#include <printf.h>
#include <riscv/csr_operations.h>
#include <riscv/extension_check.h>
#include <utils.h>

bool is_extension_implemented(enum riscv_extensions ext) {
    u32 misa = READ_CSR(misa);
    return (misa & ext);
}

// Bitwise-OR all required extensions into one register_t and pass it here
void ensure_extensions_present(const register_t req) {
    u32 machine_isa = READ_CSR(misa);
    if ((machine_isa & req) != req) {
        KERNEL_PANIC("a required extension is not implemented");
    }
    printf("[OK] All needed extensions present\n");
}

#include <klibc/panic.h>
#include <printf.h>
#include <riscv/csr_operations.h>
#include <riscv/extension_check.h>
#include <utils.h>

// NOTE: Consider moving this array definition to some higher-level code. This
// code has to just provide convenient function-wrappers for machine information
// discovery, not decide which extensions to require.
enum riscv_extensions required_riscv_extension[] = {
    RISCV_EXT_MUL_DIV,
    RISCV_EXT_USER_MODE,
};

bool is_extension_implemented(enum riscv_extensions ext) {
    u32 misa = READ_CSR(misa);
    return (misa & ext);
}

void ensure_extensions_present() {
    for (uint i = 0; i < countof(required_riscv_extension); ++i) {
        // TODO: Print more detailed info on what exactly extension is missing
        // but required by kernel.
        if (is_extension_implemented(required_riscv_extension[i]) == false) {
            KERNEL_PANIC("a required extension is not implemented");
        }
    }
    printf("[OK] All needed extensions present\n");
}

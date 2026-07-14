#include <riscv/extension_check.h>
#include <riscv/csr_operations.h>

// could've used bool for clarity?
int is_extension_implemented(enum riscv_extensions ext) {
    u32 misa = READ_CSR(misa);
    return (misa & ext);
}

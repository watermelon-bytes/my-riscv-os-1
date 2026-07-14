#include <riscv/csr_operations.h>
#include <riscv/extension_check.h>

bool is_extension_implemented(enum riscv_extensions ext) {
    u32 misa = READ_CSR(misa);
    return (misa & ext);
}

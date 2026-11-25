#include <drivers/power.h>
#include <drivers/uart.h>
#include <init_devices.h>
#include <klibc/panic.h>
#include <klibc/printf.h>
#include <mem/memory.h>
#include <mem/phys_alloc.h>
#include <riscv/csr_operations.h>
#include <riscv/extension_check.h>
#include <riscv/interrupts/interrupts.h>
#include <riscv/switch_to_supervisor.h>
#include <riscv/timer.h>

#if defined(__riscv__zbb)
    #define RISCV_OPTIONAL_ZBB RISCV_EXT_BIT_MANIPULATION
#else
    #define RISCV_OPTIONAL_ZBB 0
#endif

void* device_tree = NULL;

void kmain() {
    int hart_id;
    asm volatile("mv %0, tp" : "=r"(hart_id));
    if (hart_id == 0) {
        disable_interrupts();
        printf("[OK] Successfully switched to Supervisor Mode%c\n", '!');
        // Will already panic if any extension is missing
        // setup_interrupt_handler();
        printf("[OK] Booting on hardware thread %i\n", hart_id);
        check_device_tree(device_tree);
        init_interrupt_controller(device_tree);
        detect_memory(device_tree);
        init_phys_allocator();
        init_power_controller(device_tree);
        stimer_sleep(10000);
        printf("[FINISHED] Nothing to do left: shutting down now\n");
        /* WARNING: printf, which relies on uart_putchar, is called multiple
         * times before init_uart. Works in QEMU because I hardcoded the UART
         * address, but on other platforms, if UART isn't placed at 0x10000000,
         * then debug messages will simply waste CPU cycles and potentially
         * cause undefined behavior. Maybe add a preprocessor directive to
         * indicate whether or not target machine is QEMU virt emulator? */
        shutdown(0);
    } else {
        printf("[thread %i] halting\n", hart_id);
        halt();
    }
}

#define MSTATUS_MPP_MASK (3L << 11)  // previous mode.
#define MSTATUS_MPP_S (1L << 11)     // previous mode = Supervisor
#define SIE_SEIE (1L << 9)           // external
#define SIE_STIE (1L << 5)           // timer
__attribute__((noreturn)) void early_start(int hart_id, void* t) {
    device_tree = t;
    asm volatile("mv tp, %0" ::"r"(hart_id) : "memory");
    printf("++++++++++++++++++++++++++++\n");
    ensure_extensions_present(RISCV_EXT_USER_MODE | RISCV_EXT_MUL_DIV |
                              RISCV_EXT_SUPERVISOR_MODE);

    u32 x = READ_CSR(mstatus);
    x &= MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    WRITE_CSR(mstatus, x);

    WRITE_CSR(mepc, (uintptr_t)&kmain);
    WRITE_CSR(satp, 0);
    // TODO: maybe replace WORD_MAX with proper constants?
    WRITE_CSR(mideleg, WORD_MAX);
    WRITE_CSR(medeleg, WORD_MAX);

    WRITE_CSR(sie, READ_CSR(sie) | SIE_SEIE | SIE_STIE);

    WRITE_CSR(pmpaddr0, WORD_MAX);
    WRITE_CSR(pmpaddr1, WORD_MAX);
    WRITE_CSR(pmpcfg0, 0xf);

#define MENVCFGH_SSTC_EXTENSION (1u << 31)
    // Enable the Sstc extension to access `stimecmp` from S-mode
    WRITE_CSR(menvcfgh, READ_CSR(menvcfgh) | MENVCFGH_SSTC_EXTENSION);
    if (READ_CSR(menvcfgh) & MENVCFGH_SSTC_EXTENSION) {
        printf("[OK] SSTC extension enabled\n");
    } else {
        KERNEL_PANIC("SSTC extension missing\n");
    }

    // Give access to counters like `time`
    WRITE_CSR(mcounteren, READ_CSR(mcounteren) | (1u << 1));

    extern void s_mode_handle();
    WRITE_CSR(stvec, (uintptr_t)&s_mode_handle);
    asm volatile("mret");
    __builtin_unreachable();
}

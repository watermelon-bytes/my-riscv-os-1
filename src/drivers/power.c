// NOTE: This is now hardcoded for QEMU virt machine.
// Should be extended later to support real hardware.

#include <drivers/power.h>
#include <klibc/panic.h>
#include <klibc/types.h>
#include <libfdt.h>

void (*shutdown)(int);

void (*reboot)();

static volatile u32* const syscon_base = (volatile u32*)0x100000;

enum {
    SYSCON_SHUTDOWN = 0x5555,
    SYSCON_REBOOT = 0x7777,
    SYSCON_PANIC = 0x3333
};

static void syscon_shutdown(int exit_code) {
    *syscon_base = (exit_code << 16) | SYSCON_SHUTDOWN;
}

static void syscon_reboot() { *syscon_base = SYSCON_REBOOT; }

void init_power_controller(void* device_tree) {
    (void)device_tree;
    shutdown = &syscon_shutdown;
    reboot = &syscon_reboot;
}

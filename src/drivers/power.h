#pragma once

extern void (*shutdown)(int possible_exit_code);

extern void (*reboot)();

void init_power_controller(void* device_tree);

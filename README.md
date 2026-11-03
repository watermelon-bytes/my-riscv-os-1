# my-riscv-os-1

A minimal RISC-V kernel written in C.
Doesn't require a SBI like OpenSBI, but the bootloader has to pass a valid Device Tree.

### System requirements to build

- RISC-V GNU C & C++ toolchain (grab it (here)[https://github.com/riscv-collab/riscv-gnu-toolchain])
- CMake >=3.16

To run in QEMU, ensure you have the `qemu-system-riscv32` emulator installed.

### Build with CMake

To build the kernel, use the default CMake build steps:
```sh 
cmake -S . -B bin/
cmake --build bin/ 
```



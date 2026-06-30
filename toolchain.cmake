# toolchain.cmake - RISC-V freestanding toolchain

set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  riscv32)

# Cross compiler
set(CMAKE_C_COMPILER        riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER      riscv32-unknown-elf-g++)
set(CMAKE_ASM_COMPILER      riscv32-unknown-elf-gcc)
set(CMAKE_OBJCOPY           riscv32-unknown-elf-objcopy)
set(CMAKE_OBJDUMP           riscv32-unknown-elf-objdump)
set(CMAKE_SIZE              riscv32-unknown-elf-size)

# Linker
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# We are freestanding / bare-metal
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Optional: Force no standard library (you can override in CMakeLists if needed)
add_compile_options(-ffreestanding -nostdlib)

cmake_minimum_required(VERSION 3.14)

project(riscv-test LANGUAGES C ASM)

# Ensure the host machine has the necessary toolchain available
include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the libfdt library from subdirectory
add_subdirectory(libfdt)

file(GLOB_RECURSE SOURCES
    "src/**.c"
    "src/**.S"
)
add_executable(${PROJECT_NAME} ${SOURCES})

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     C_STANDARD 11
#     C_STANDARD_REQUIRED YES
#     C_EXTENSIONS NO
# )
# Link against libfdt
target_link_libraries(${PROJECT_NAME} PRIVATE fdt)

target_link_options(${PROJECT_NAME} PRIVATE
    "-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
    -nostdlib -lgcc
)

# Make sure the linker script is visible in IDEs
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    PROPERTIES HEADER_FILE_ONLY TRUE)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libfdt
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    -mcmodel=medany
    -ffreestanding
    -march=rv32imacd_zicsr
    -nostartfiles
    -mabi=ilp32d
)

set(QEMU_FLAGS
    -machine virt
    -bios none
    -kernel $<TARGET_FILE:${PROJECT_NAME}>
    -serial mon:stdio
)

add_custom_target(qemu-run
    COMMAND qemu-system-riscv32
        ${QEMU_FLAGS}
    DEPENDS ${PROJECT_NAME}
    USES_TERMINAL
    COMMENT "Running kernel in QEMU (RISC-V 32-bit virt machine)"
)


add_custom_target(qemu-debug
    COMMAND qemu-system-riscv32
        ${QEMU_FLAGS}
        -S -s
    DEPENDS ${PROJECT_NAME}
    USES_TERMINAL
    COMMENT "Running kernel in QEMU [waiting for GDB]"
)

cmake_minimum_required(VERSION 3.15)

add_library(fdt STATIC
    fdt_addresses.c
    fdt.c
    fdt_check.c
    fdt_empty_tree.c
    fdt.h
    fdt_overlay.c
    fdt_ro.c
    fdt_rw.c
    fdt_strerror.c
    fdt_sw.c
    fdt_wip.c
    libfdt_env.h
    libfdt.h
    libfdt_internal.h
)

target_include_directories(fdt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(fdt PUBLIC
    -Wall -Wextra -ffreestanding -nostdlib
)

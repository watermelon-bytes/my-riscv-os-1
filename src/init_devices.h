#pragma once
#include <stdbool.h>
#include <types.h>
#include <stdint.h>
#include <stddef.h>
_Bool check_device_tree(void const*);

int init_devices(void* fdt);

/*
 * Looks up name of the specified node in the array of possible names, then
 * returns a pointer to nececcary function-initializer or NULL if
 * - such a node name wasn't found
 * - or libfdt returned an error
 * - or initializer isn't developed / ready to use yet in the present kernel
 * version
 *
 * @param[in] name: node name to find
 */
typedef int (*device_initializer_func)(const void* tree, int node);
device_initializer_func get_device_initializer(const char* name);

/**
 * Reads and caches the `#address-cells` and `#size-cells` sizes from the
 * root node. Must be called exactly once before invoking any other parsing
 * functions that rely on cell sizes.
 *
 * @return 0 on success.
 * @return 1 if the configuration has already been initialized.
 * @return < 0 libfdt error code if an error occurs during lookup.
 *
 * TODO: Consider halting if called a second time to catch debugging issues
 * early.
 */
int configure_field_size(void* fdt, int root);

/*
 * @tree Pointer to device tree
 * @node Offset of the node containing the necessary <reg> field
 * @begin_buf Pointer to buffer where start address will be stored
 * @size_buf Pointer to buffer where size will be stored
 *
 * Requires configure_field_size to be called beforehand as it relies on values
 * stored in variables by configure_field_size.
 */
void parse_reg(void* tree, const int node, uintptr_t* begin_buf,
               size_t* size_buf);

/**
 * Safely parses a Device Tree Address cell array into a native
 * pointer.
 *
 * This function handles cases where the Device Tree has cell sizes larger
 * than the platform's native pointer width (e.g., 64-bit DTB properties
 * on a 32-bit system). If the value fits within the native pointer width, it is
 * extracted; otherwise, it fails.
 *
 * @note Assumes a minimum native pointer size of 32 bits.
 * @note Assumes the input cell data is encoded in Big-Endian (Device Tree
 * standard).
 *
 * @param[out] result  Pointer to a variable where the parsed native address
 * will be stored. Only written to if the function returns true.
 * @param[in]  cells   Pointer to the raw DT cell array representing the
 * address or size.
 *
 * Returns NULL on error, and pointer to next cell after the address on success.
 */
const u32* fetch_native_pointer(uintptr_t* const result, const uint32_t* cells);
const u32* fetch_size_field(register_t* result, const u32* cells);

int get_sizeof_one_descriptor();

int detect_memory(void* fdt);
int detect_power_config(void* fdt);
int parse_systems_on_chip(void* fdt);

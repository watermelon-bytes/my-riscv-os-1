#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>
_Bool check_device_tree(void const*);

int init_devices(void* fdt);

/* WARNING: initialization policy has changed. Now the kernel doesn't iterate
 * through device tree and doesn't call initializers for each node, but rather
 * device drivers lookup necessary nodes and fetch platform-specific
 * information. Hence this function is unnecessary.
 *
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
 * WARNING: this function is unnecessary. Consider a removal.
 *
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
 */
enum parsing_error parse_reg(const void* tree, const int node,
                             uintptr_t* begin_buf, size_t* size_buf);

int get_sizeof_one_descriptor();

int detect_memory(const void* fdt);

enum parsing_error {
    SUCCESS,
    NO_SUCH_FIELD,
    OUT_OF_SLOTS,
    ORIG_VAL_TOO_BIG,
    CALL_AGAIN
};

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
_Bool check_device_tree(void const*);
int init_devices(void* fdt);

_Bool parse_device_info(const char* name);

/*
 * Reads #address-cells and #size-cells in the root node and stores them in
 * static variables It is mandatory to call `configure_field_size` before any
 * parsing functions, or they will fail
 */
_Bool configure_field_size(void* fdt);

/*
 * @begin_buf Buffer where start address will be stored
 * @size_buf Buffer where size will be stored
 * Requires configure_field_size to be called before as it relies on values
 * stored in variables by configure_field_size.
 */
void parse_reg(const uint32_t* reg, uintptr_t* begin_buf, size_t* size_buf);


#include <types.h>

/* TODO: Make a physical page allocator, which returns a signle new free page in
 * RAM or -1 if can't find it.
 * It should rely on the RAM regions that have been detected by detect_memory()
 * That is, the allocator has to understand how much memory left, define page
 * size. From the very beginning, it makes a bitmap or a linked list so that it
 * fits the memory amount and matches pages count
 * Allocator has to know where kernel data lays, so it won't clobber those
 * regions. Therefore linker must provide such symbols as "beginning" and "end",
 * pointing to kernel binary image boundaries in RAM.
 *
 * TODO: decide what to use for allocator, either bitmap or list
 * --- Bitmap: pros and cons ---
 * + saves a lot of space
 * + more cache-friendly
 * + easier to debug - everything is in one place
 * - more complex to initialize properly: taking into
 * account kernel image boundaries and machine-dependent compile-time-unknown
 * RAM size
 * -
 * --- Linked list ---
 * + possible easier to implement and manage
 * + does not depend on total count of pages
 * - requires **considerably** more runtime initialization
 */
extern u8 _kernel_physical_start[];
extern u8 _kernel_physical_end[];

void init_phys_mem_allocator();

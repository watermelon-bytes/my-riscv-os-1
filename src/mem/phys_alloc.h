#pragma once
#include <limits.h>
#include <types.h>

#if __STDC_VERSION__ >= 202311l
constexpr uint PAGE_OFFSET_BITS = 12;
constexpr uint PAGE_SIZE = 1 << PAGE_OFFSET_BITS;
constexpr uint PAGE_MASK = (UINT_MAX << PAGE_OFFSET_BITS);
constexpr uint OFFSET_MASK = ~(UINT_MAX << PAGE_OFFSET_BITS);
#else
enum : register_t {
    PAGE_OFFSET_BITS = 12,
    PAGE_SIZE = 1 << PAGE_OFFSET_BITS,
    PAGE_MASK = (UINT_MAX << PAGE_OFFSET_BITS),
    OFFSET_MASK = ~PAGE_MASK,
};
#endif
#define PAGE_ALIGNED(x) ((x) & PAGE_MASK)

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
 *
 * Conclusion: Bitmap wins
 */
extern uintptr_t _kernel_physical_start;
extern uintptr_t _kernel_physical_end;

// Places and initializes a page bitmap in first available page
// Assumes that the only borrowed memory is where kernel image is placed - i.e.
// range [_kernel_physical_start -> _kernel_physical_end]
// Relies on RAM info provided by Device Tree and parsed by functions in
// memory.c
void init_phys_allocator();

void free_physical_page(void*);

void* allocate_page();

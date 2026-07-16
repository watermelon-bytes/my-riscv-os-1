#include <klibc/panic.h>
#include <klibc/utils.h>
#include <mem/memory.h>
#include <mem/phys_alloc.h>
#include <string.h>
#ifndef NDEBUG
    #define LOG_VARIABLE(var, type) ({ printf(#var " = " type "\n", var); })
#else
    #define LOG_VARIABLE(var, type) ;
#endif

static int available_pages_count = 0;
static u8* bitmap = NULL;

size_t get_kernel_size() {
    return _kernel_physical_end - _kernel_physical_start;
}

void init_phys_allocator() {
    /* TODO: Consider calling this function from detect_memory() or make a
     * function init_memory() that will call detect_memory() first and
     * init_phys_allocator() later, so that the initialization order can be
     * tracked and managed easily
     *
     * 1. Detect available memory regions and total size
     * 2. Calculate size of bitmap and allocate physically and logically
     * continuous space (for simplicity, we won't handle the case where it is
     * possible to allocate a non-continuous space but impossible to fit bitmap
     * into a single block)
     * 3. Exclude the space where kernel image is placed
     * 4. Clear all page bits to zeroes; enable bits conforming to kernel
     * image pages.
     */
    const __auto_type total_mem = get_total_mem();
    // We assume that memory amount is 4Kib-aligned
    const long unborrowed = total_mem - get_kernel_size();
    if (unborrowed < (signed)PAGE_SIZE * 6) {
        KERNEL_PANIC("not enough RAM available (%u bytes only)", unborrowed);
    }
    // NOTE: Consider using division instead of bitwise shift for better
    // readability
    const __auto_type total_pages_available = total_mem >> PAGE_OFFSET_BITS;
    // bitmap size = total pages count / bits per byte
    const signed bitmap_size = total_pages_available / CHAR_BIT +
                               (total_pages_available % CHAR_BIT ? 1 : 0);
    LOG_VARIABLE(bitmap_size, "%i");
    for (uint i = 0; i < ram_regions_index; ++i) {
        __auto_type const region = &ram_regions[i];
        intptr_t tmp = (intptr_t)_kernel_physical_start - region->physicaddr;
        if (tmp >= bitmap_size) {
            bitmap = (u8*)PAGE_ALIGNED(region->physicaddr);
            break;
        }
        tmp = PAGE_ALIGNED(region->physicaddr + region->space_size) -
              (intptr_t)_kernel_physical_end;
        if (tmp >= bitmap_size) {
            LOG_VARIABLE(_kernel_physical_end, "0x%x");
            bitmap = (u8*)_kernel_physical_end;
            break;
        }
    }
    // Panic if could not allocate enough continuous space
    if (bitmap == NULL) {
        KERNEL_PANIC(
            "can't find continuous space for bitmap; non-continuous page "
            "bitmaps not supported yet");
    }
    // word_aligned_memset(bitmap, 0, bitmap_size);
    available_pages_count = unborrowed >> PAGE_OFFSET_BITS;
    printf(
        "Bitmap successfully located at 0x%x, size = 0x%x; %i pages "
        "available\n",
        (uintptr_t)bitmap, (unsigned)bitmap_size, available_pages_count);
    // TODO: Mark kernel pages as "used"
}

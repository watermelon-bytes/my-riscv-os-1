#include <klibc/panic.h>
#include <klibc/utils.h>
#include <limits.h>
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
static int bitmap_size = 0;

size_t get_kernel_size() {
    return _kernel_physical_end - _kernel_physical_start;
}

#define BITS_COUNT(x) (sizeof(x) * CHAR_BIT)
#define WORD_ALIGNED(x) \
    ((register_t*)((uintptr_t)(x) & ~(sizeof(register_t) - 1)))

static void mark_as_used(const uint first_page_index, size_t total_pages) {
    LOG_VARIABLE(bitmap_size * CHAR_BIT, "%i");
    LOG_VARIABLE(first_page_index, "%u");
    LOG_VARIABLE(total_pages, "%lu");
    ASSERT(first_page_index + total_pages < bitmap_size * CHAR_BIT);
    ASSERT(bitmap != NULL);

    const uint offset = first_page_index % CHAR_BIT;
    register_t* curr_word = WORD_ALIGNED(&bitmap[first_page_index / CHAR_BIT]);
    if (offset + total_pages < BITS_COUNT(register_t)) {
        const register_t chunk = UINTPTR_MAX << (offset + total_pages);
        *curr_word |= chunk >> offset;
        return;
    }
    total_pages -= offset;
    *curr_word |= UINTPTR_MAX >> offset;
    for (++curr_word; total_pages >= BITS_COUNT(register_t);
         total_pages -= BITS_COUNT(register_t)) {
        *curr_word++ = UINTPTR_MAX;
    }
    *curr_word |= UINTPTR_MAX << (BITS_COUNT(register_t) - total_pages);
}
#undef BITS_COUNT
#undef WORD_ALIGNED

void init_phys_allocator() {
    /* ensure we're called AFTER detect_memory() */
    ASSERT(ram_regions_index > 0);
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
    bitmap_size = total_pages_available / CHAR_BIT +
                  (total_pages_available % CHAR_BIT ? 1 : 0);
    LOG_VARIABLE(bitmap_size, "%i");
    for (uint i = 0; i < ram_regions_index; ++i) {
        __auto_type const region = &ram_regions[i];
        intptr_t tmp = (intptr_t)_kernel_physical_start - region->physicaddr;
        LOG_VARIABLE(_kernel_physical_start, "0x%p");
        if (tmp >= bitmap_size) {
            bitmap = (u8*)PAGE_ALIGNED(region->physicaddr);
            break;
        }
        tmp = PAGE_ALIGNED(region->physicaddr + region->space_size) -
              (intptr_t)_kernel_physical_end;
        LOG_VARIABLE(_kernel_physical_end, "0x%p");
        if (tmp >= bitmap_size) {
            bitmap = _kernel_physical_end;
            break;
        }
    }
    // Panic if could not allocate enough continuous space
    if (bitmap == NULL) {
        KERNEL_PANIC(
            "can't find continuous space for bitmap; non-continuous page "
            "bitmaps not supported yet");
    }
    ASSERT(PAGE_ALIGNED(bitmap) == (uintptr_t)bitmap);
    available_pages_count = unborrowed >> PAGE_OFFSET_BITS;
    printf(
        "[OK] Bitmap successfully located at 0x%p - 0x%p; %i pages available\n",
        bitmap, (bitmap + bitmap_size), available_pages_count);
    word_aligned_memset(bitmap, 0, bitmap_size);
    mark_as_used((uintptr_t)_kernel_physical_start >> PAGE_OFFSET_BITS,
                 get_kernel_size());
    printf("[OK] Initialized bitmap\n");
}

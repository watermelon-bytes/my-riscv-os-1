#include <klibc/bitmap.h>
#include <klibc/bitwise_utils.h>
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

static struct bitmap physical_bitmap_ = {.slots_ptr = NULL};

size_t get_kernel_size() {
    return _kernel_physical_end - _kernel_physical_start;
}

/*
 * @page A page-aligned pointer to page whose number is to be defined
 * Finds the corresponding page number in bitmap
 */
static int find_page_by_physical_addr(const void* page) {
    const uintptr_t page_ptr = (uintptr_t)page & (~OFFSET_MASK);
    uint counter = 0;
    for (uint i = 0; i < total_memory_regions(); ++i) {
        const size_t size = get_memory_region(i).space_size;
        const uintptr_t phys_addr = get_memory_region(i).physicaddr;
        if (page_ptr >= phys_addr && page_ptr <= phys_addr + size) {
            const size_t diff = page_ptr - phys_addr;
            return counter + (diff >> PAGE_OFFSET_BITS);
        }
        counter += get_memory_region(i).space_size / PAGE_SIZE;
    }
    return -1;
}

static void* find_physical_addr_of_page(uint ppn) {
    for (uint i = 0; i < total_memory_regions(); ++i) {
        const __auto_type pages_in_this_reg =
            get_memory_region(i).space_size >> PAGE_OFFSET_BITS;
        if (ppn > pages_in_this_reg) {
            return (void*)(get_memory_region(i).physicaddr +
                           (ppn << PAGE_OFFSET_BITS));
        }
    }
    return NULL;
}

// static void mark_as_used(const void* page, size_t total_pages) {
//     // TODO: Instead of ASSERT's, return error code

//     ASSERT(bitmap.slots_ptr != NULL);
//     const int ppn = find_page_by_physical_addr(page);
//     ASSERT(ppn >= 0);
//     ASSERT(ppn + total_pages < bitmap_size);
//     const uint offset = ppn & (WORD_SIZE - 1);
//     register_t* curr_word = &bitmap[ppn / WORD_SIZE];
//     if (offset + total_pages < WORD_SIZE) {
//         const register_t chunk = UINTPTR_MAX << (offset + total_pages);
//         *curr_word |= chunk >> offset;
//         return;
//     }
//     total_pages -= offset;
//     *curr_word |= UINTPTR_MAX >> offset;
//     for (++curr_word; total_pages >= BITS_COUNT(register_t);
//          total_pages -= WORD_SIZE) {
//         *curr_word++ = UINTPTR_MAX;
//     }
//     *curr_word |= UINTPTR_MAX << (WORD_SIZE - total_pages);
// }
#undef BITS_COUNT
#undef WORD_ALIGNED

void init_phys_allocator() {
    /* ensure we're called AFTER detect_memory() */
    ASSERT(total_memory_regions() > 0);
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
    const i32 total_mem = get_total_mem();
    // We assume that memory amount is 4Kib-aligned
    const long unborrowed = total_mem - get_kernel_size();
    if (unborrowed < (signed)PAGE_SIZE * 6) {
        KERNEL_PANIC("not enough RAM available (%u bytes only)", unborrowed);
    }
    // NOTE: Consider using division instead of bitwise shift for better
    // readability
    const __auto_type total_pages_available = total_mem >> PAGE_OFFSET_BITS;

    // bitmap size = total pages count / bits per byte
    const size_t bitmap_size = total_pages_available / CHAR_BIT +
                               (total_pages_available % CHAR_BIT ? 1 : 0);
    // LOG_VARIABLE(bitmap_size, "%i");
    void* slots_for_bitmap = NULL;

    const uintptr_t kern_start = (uintptr_t)_kernel_physical_start,
                    kern_end = (uintptr_t)_kernel_physical_end;
    LOG_VARIABLE(kern_start, "0x%p");
    LOG_VARIABLE(kern_end, "0x%p");

    for (uint i = 0; i < total_memory_regions(); ++i) {
        const struct ram_descriptor region = get_memory_region(i);
        const uintptr_t region_end = region.physicaddr + region.space_size;
        LOG_VARIABLE(region.physicaddr, "0x%x");

        __auto_type next_boundary = region_end;
        if (kern_start >= region.physicaddr && kern_start < region_end) {
            next_boundary = kern_start;
        }
        if (next_boundary - region.physicaddr >= bitmap_size) {
            slots_for_bitmap = (void*)region.physicaddr;
            break;
        }
        if (region_end - kern_end >= bitmap_size) {
            slots_for_bitmap = _kernel_physical_end;
            break;
        }
    }
    // Panic if could not allocate enough continuous space
    if (slots_for_bitmap == NULL) {
        KERNEL_PANIC(
            "can't find continuous space for bitmap; non-continuous page "
            "bitmaps not supported yet");
    }
    printf("[OK] Placed bitmap at 0x%p - 0x%p\n", slots_for_bitmap,
           (u8*)slots_for_bitmap + bitmap_size);
    bitmap_init(&physical_bitmap_, slots_for_bitmap, bitmap_size);
    printf("[OK] Initialized bitmap\n");
}

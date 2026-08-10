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
static int physic_addr_to_ppn(const void* page) {
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

/*
 * Returns physical address for the specified Physical Page Number.
 * The reason why this isn't just a bitwise right-shift is that since there
 * might be multiple regions of RAM, you can't simply take one base physical
 * address: the bitmap places regions ordered from the lowest address to the
 * highest, but doesn't allocate bits the invalid space between regions.
 * Thus, if you have one region [0x10,000 -> 0x20,000] and another [0x35,000 ->
 * 0x45,000], then first PPN will be assigned to the page at 0x10,000; but PPN
 * 11 (with size of page = 0x1,000) will mean the page at 0x35,000, not at
 * 0x21,000 as this isn't a physical RAM addressable space.
 *
 * @ppn Number of Physical Page whose physical address is to be returned
 */
static void* find_physical_addr_of_page(uint ppn) {
    for (uint i = 0; i < total_memory_regions(); ++i) {
        const size_t pages_in_this_reg =
            get_memory_region(i).space_size >> PAGE_OFFSET_BITS;
        if (ppn > pages_in_this_reg) {
            return (void*)(get_memory_region(i).physicaddr +
                           (ppn << PAGE_OFFSET_BITS));
        }
    }
    return NULL;
}

/*
 * Marks the pages that the specified region [page_phys_addr, page_phys_addr +
 * total_bytes) covers.
 */
static void pmm_borrow_pages(const void* page_phys_addr, size_t total_bytes) {
    const int ppn = physic_addr_to_ppn(page_phys_addr);
    const size_t pages_to_borrow =
        total_bytes / PAGE_SIZE + (total_bytes % PAGE_SIZE ? 1 : 0);
    printf(
        "[pmm_borrow_pages] Marking %i pages as used, starting at page no. "
        "0x%x\n",
        pages_to_borrow, ppn);
    bitmap_mark_as_used(&physical_bitmap_, ppn, pages_to_borrow);
}

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
    ASSERT(get_kernel_size() % PAGE_SIZE == 0);
    const long unborrowed = total_mem - get_kernel_size();
    if (unborrowed < (signed)PAGE_SIZE * 6) {
        KERNEL_PANIC("not enough RAM available (%u bytes only)", unborrowed);
    }
    // NOTE: Consider using division instead of bitwise shift for better
    // readability
    const __auto_type total_pages_available = total_mem >> PAGE_OFFSET_BITS;

    // bitmap size = total pages count / bits in word
    const size_t bitmap_size = total_pages_available / WORD_SIZE +
                               (total_pages_available % WORD_SIZE ? 1 : 0);
    LOG_VARIABLE(bitmap_size, "%i");
    void* slots_for_bitmap = NULL;

    const uintptr_t kern_start = (uintptr_t)_kernel_physical_start,
                    kern_end = (uintptr_t)_kernel_physical_end;
    LOG_VARIABLE(kern_start, "0x%p");
    LOG_VARIABLE(kern_end, "0x%p");

    const size_t bitmap_size_in_bytes = bitmap_size * sizeof(word_t);
    for (uint i = 0; i < total_memory_regions(); ++i) {
        LOG_VARIABLE(bitmap_size_in_bytes, "%u");
        const struct ram_descriptor region = get_memory_region(i);
        const uintptr_t region_end = region.physicaddr + region.space_size;
        LOG_VARIABLE(region.physicaddr, "0x%x");

        __auto_type next_boundary = region_end;
        if (kern_start >= region.physicaddr && kern_start < region_end) {
            next_boundary = kern_start;
        }
        if (next_boundary - region.physicaddr >= bitmap_size_in_bytes) {
            slots_for_bitmap = (void*)region.physicaddr;
            break;
        }
        if (region_end - kern_end >= bitmap_size_in_bytes) {
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
    pmm_borrow_pages(_kernel_physical_start, get_kernel_size());
    pmm_borrow_pages(slots_for_bitmap, bitmap_size);
    printf("[OK] Initialized bitmap\n");
}

void* allocate_page() {
    const int slot = bitmap_allocate_slot(&physical_bitmap_);
    if (slot == -1) {
        return NULL;
    }
    return find_physical_addr_of_page(slot);
}

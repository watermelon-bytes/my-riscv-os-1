// Common interface for bitmaps
#pragma once
#include <types.h>

struct bitmap {
    /* Contains size of array at *slots_ptr, i.e. how many 32-bit or 64-bit
     * chunks we have */
    size_t slots_size;

    /* Total zeroed bits */
    uint free_slots_count;

    /* Index of word in slots_ptr that (potentially) has clear bits */
    uint last_freed;

    /* Pointer to runtime-allocated array. Could have used C23 FMA but this is
     * more backwards-compatible option */
    register_t* slots_ptr;
};

/*
 * Initializes the bitmap pointed to by @map with all zeroes
 * @slots Word-aligned buffer that belongs to the bitmap from now on.
 * @size Size of slots buffer **in words** (i.e. if buffer length = 64 bytes,
 * and word size is 4 bytes, then size must be 16)
 */
void bitmap_init(struct bitmap*, word_t slots[], size_t size);

/*
 * Finds a free slot in the bitmap, marks it as borrowed, and returns the slot
 * index
 */
int bitmap_allocate_slot(struct bitmap*);

void bitmap_free_slot(struct bitmap*, uint slot);

/*
 * Marks `slots_to_mark` slots, starting from `first_slot`, as used.
 */
void bitmap_mark_as_used(struct bitmap*, uint first_slot, u32 slots_to_mark);

_Bool bitmap_is_slot_borrowed(struct bitmap* bm, const size_t slot_no);

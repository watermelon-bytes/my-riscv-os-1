// Common interface for bitmaps
#pragma once
#include <types.h>

struct bitmap {
    int total;
    uint free_slots_count;
    uint last_freed;
    register_t* slots_ptr;
};

/*
 * Initializes the bitmap pointed to by @map with all zeroes
 * @slots Word-aligned buffer that belongs to the bitmap from now on.
 * @size Size of slots buffer in words (i.e. if buffer length = 64 bytes, and
 * word size is 4 bytes, then size must be 16)
 */
void bitmap_init(struct bitmap*, register_t slots[], size_t size);

// Returns slot no.
int bitmap_allocate_slot(struct bitmap*);

void bitmap_free_slot(struct bitmap*, uint slot);

void bitmap_mark_as_used(struct bitmap*, uint first_slot, i32 slots_to_mark);

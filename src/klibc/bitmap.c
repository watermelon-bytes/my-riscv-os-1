// Common interface for bitmaps
#include <bitwise_utils.h>
#include <klibc/bitmap.h>
#include <klibc/panic.h>
#include <klibc/utils.h>

bool bitmap_is_slot_borrowed(struct bitmap* bm, const size_t slot_no) {
    return bm->slots_ptr[slot_no / WORD_SIZE] & (1 << (slot_no % WORD_SIZE));
}

/*
 * Internal function to set a specific bit in bitmap to 1.
 * @index position of the bit to enable
 */
// TODO: make the function static when build type isn't "test"
bool _enable_bit(struct bitmap* bm, const size_t index) {
    const size_t word_index = index / WORD_SIZE, bit_index = index % WORD_SIZE;
    if (word_index < bm->slots_size) {
        bm->slots_ptr[word_index] |= (word_t)(1u << bit_index);
        return true;
    }
    return false;
}

void bitmap_init(struct bitmap* map, register_t slots[], size_t size) {
    ASSERT(slots != NULL);
    if (slots == NULL || size == 0) {
        return;
    }
    map->slots_ptr = slots;
    map->slots_size = size;
    map->free_slots_count = size * WORD_SIZE;
    map->last_freed = 0;
    word_aligned_memset(slots, 0, size);
}

int bitmap_allocate_slot(struct bitmap* map) {
    ASSERT(map != NULL);
    for (size_t i = 0; i < map->slots_size; ++i) {
        if (map->slots_ptr[i] == WORD_MAX) {
            continue;
        } else {
            reg_t* slot = &map->slots_ptr[i];
            const uint offset = count_trailing_zeroes(~*slot);
            *slot |= 1 << offset;
            map->free_slots_count--;
            return WORD_SIZE * i + offset;
        }
    }
    return -1;
}

void bitmap_free_slot(struct bitmap* map, uint slot) {
    ASSERT(map->slots_size > slot);
    const uint word = slot / WORD_SIZE, offset = slot % WORD_SIZE;
    const word_t snapshot = map->slots_ptr[word];
    map->slots_ptr[word] &= ~(1 << offset);
    if (map->slots_ptr[word] != snapshot) {
        map->free_slots_count++;
    }
}

void bitmap_mark_as_used(struct bitmap* bitmap, uint first_slot,
                         u32 slots_to_mark) {
    const __auto_type last_slot = first_slot + slots_to_mark;
    for (uint i = first_slot; i < last_slot; ++i) {
        _enable_bit(bitmap, i);
    }
    return;
    /* TODO: below is the previous version of this function. The current
     * function implementation is rather a fallback than a solution, the code
     * below needs to be fixed. */

    // uint word_index = first_slot / WORD_SIZE, rem = first_slot % WORD_SIZE;
    // if (slots_to_mark < 1 || bitmap->slots_ptr == NULL ||
    //     word_index > bitmap->slots_size) {
    //     return;
    // }
    // if (rem) {
    //     bitmap->slots_ptr[word_index] |= WORD_MAX >> (WORD_SIZE -
    //     first_slot); word_index++, slots_to_mark -= rem;
    // }
    //
    // while (slots_to_mark >= WORD_SIZE && word_index < bitmap->slots_size) {
    //     bitmap->slots_ptr[word_index] = WORD_MAX;
    //     slots_to_mark -= WORD_SIZE, ++word_index;
    // }
    //
    // if (slots_to_mark > 0) {
    //     bitmap->slots_ptr[word_index] |= WORD_MAX
    //                                      << (WORD_SIZE - slots_to_mark);
    // }
}

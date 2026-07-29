// Common interface for bitmaps
#include <bitwise_utils.h>
#include <klibc/bitmap.h>
#include <klibc/panic.h>
#include <klibc/utils.h>

void bitmap_init(struct bitmap* map, register_t slots[], size_t size) {
    ASSERT(slots != NULL);
    if (slots == NULL || size == 0) {
        return;
    }
    map->slots_ptr = slots;
    map->total = size;
    map->free_slots_count = size * WORD_SIZE;
    map->last_freed = 0;
    word_aligned_memset(slots, 0, size);
}

int bitmap_allocate_slot(struct bitmap* map) {
    ASSERT(map != NULL);
    for (size_t i = 0; i < map->total; ++i) {
        if (map->slots_ptr[i] == WORD_MAX) {
            continue;
        } else {
            const uint offset = count_trailing_zeroes(map->slots_ptr[i]);
            map->slots_ptr[i] |= 1 << offset;
            map->free_slots_count--;
            return WORD_SIZE * i + offset;
        }
    }
    return -1;
}

void bitmap_free_slot(struct bitmap* map, uint slot) {
    ASSERT(map->total > slot);
    const uint word = slot / WORD_SIZE, offset = slot % WORD_SIZE;
    const word_t snapshot = map->slots_ptr[word];
    map->slots_ptr[word] &= ~(1 << offset);
    if (map->slots_ptr[word] != snapshot) {
        map->free_slots_count++;
    }
}

void bitmap_mark_as_used(struct bitmap* bitmap, uint first_slot,
                         i32 slots_to_mark) {
    ASSERT(bitmap != NULL);
    ASSERT(bitmap->slots_ptr != NULL);
    if (slots_to_mark < 1) {
        return;
    }
    uint word_index = first_slot / WORD_SIZE, rem = first_slot % WORD_SIZE;
    if (rem) {
        bitmap->slots_ptr[word_index] |= WORD_MAX >> (WORD_SIZE - first_slot);
        word_index++;
        slots_to_mark -= rem;
    }

    while (slots_to_mark >= WORD_SIZE) {
        bitmap->slots_ptr[word_index] = WORD_MAX;
        slots_to_mark -= WORD_SIZE;
        ++word_index;
    }

    if (slots_to_mark > 0) {
        bitmap->slots_ptr[word_index] |= WORD_MAX
                                         << (WORD_SIZE - slots_to_mark);
    }
}

#include <klibc/bitmap.h>
#include <klibc/panic.h>

#define SLOTS_SIZE 10
reg_t slots[SLOTS_SIZE];
struct bitmap my_bitmap;

static void debug_print_bitmap(struct bitmap* bm) {
    printf("----------------\n");
    for (size_t i = 0; i < bm->slots_size; ++i) {
        for (size_t j = 0; j < WORD_SIZE; ++j) {
            const uint bit = (bm->slots_ptr[i] >> j) & 1;
            _putchar('0' + bit);
        }
        _putchar('\n');
    }
    printf("----------------\n");
}

bool test_enable_bit() {
    extern bool _enable_bit(struct bitmap*, const size_t);
    const size_t off = 25, index = 5 * WORD_SIZE;
    const reg_t result = 1u << off;
    _enable_bit(&my_bitmap, index + off);
    return slots[5] & result;
}

bool test_mark_as_used() {
    bitmap_mark_as_used(&my_bitmap, 0, 50);
    bitmap_mark_as_used(&my_bitmap, 55, 5);
    size_t i = 0;
    for (; i < 50; ++i) {
        ASSERT(bitmap_is_slot_borrowed(&my_bitmap, i) == 1);
    }
    for (; i < 55; ++i) {
        ASSERT(bitmap_is_slot_borrowed(&my_bitmap, i) == 0);
    }
    for (; i < 60; ++i) {
        ASSERT(bitmap_is_slot_borrowed(&my_bitmap, i) == 1);
    }
    for (; i < WORD_SIZE * SLOTS_SIZE; ++i) {
        ASSERT(bitmap_is_slot_borrowed(&my_bitmap, i) == 0);
    }
    debug_print_bitmap(&my_bitmap);
    return true;
}

bool test_mark_as_used2() {
    bitmap_mark_as_used(&my_bitmap, 0, 12);
    bitmap_mark_as_used(&my_bitmap, 12, 2);
    printf("%b\n", slots);
    return slots[0] == 0b111111111111111111;
}

void run_all_tests() {
    bitmap_init(&my_bitmap, slots, SLOTS_SIZE);
    ASSERT(my_bitmap.slots_size = SLOTS_SIZE);
    ASSERT(test_mark_as_used());
    printf("[OK] All tests passed!");
}

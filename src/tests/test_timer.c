#include <klibc/panic.h>
#include <klibc/utils.h>
#include <stdbool.h>

static void handle() {}

static bool test_small_delay() { return true; }

__attribute__((used, noinline))  //
void test_timer_all() {}

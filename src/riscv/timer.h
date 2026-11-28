#pragma once
#include <klibc/types.h>

/* Sets time comparator CSR to sum of the current time and the speicified in the
 * first parameter count of milliseconds */
void stimer_sleep(const uint cycles);

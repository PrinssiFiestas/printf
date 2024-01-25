#ifndef PRINTF_H_INCLUDED
#define PRINTF_H_INCLUDED 1

#include <gpc/attributes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

// Max decimal digits in uintmax_t. Used for allocating itoa char buffers.
#if CHAR_BIT == 8
#define MAX_DIGITS (sizeof(uintmax_t) * 3)
#else // what kind of crazy system are you running where char is not 8 bits??
#define MAX_DIGITS ((CHAR_BIT * sizeof(uintmax_t) * 3) / 8)
#endif

#endif // PRINTF_H_INCLUDED

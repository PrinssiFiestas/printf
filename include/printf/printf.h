#ifndef PRINTF_H_INCLUDED
#define PRINTF_H_INCLUDED 1

#include <gpc/attributes.h>
#include <stdio.h>
#define __STDC_WANT_IEC_60559_BFP_EXT__
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>

// Max decimal digits in uintmax_t. Used for allocating itoa char buffers.
#if CHAR_BIT == 8
#define MAX_DIGITS (sizeof(uintmax_t) * 3)
#else // what kind of crazy system are you running where char is not 8 bits??
#define MAX_DIGITS ((CHAR_BIT * sizeof(uintmax_t) * 3) / 8)
#endif

unsigned pf_vsprintf(char buf[static 1], const char fmt[static 1], va_list args);

__attribute__ ((format (printf, 2, 3)))
unsigned pf_sprintf(char buf[static 1], const char fmt[static 1], ...);

#endif // PRINTF_H_INCLUDED

#ifndef PRINTF_H_INCLUDED
#define PRINTF_H_INCLUDED 1

#include <gpc/attributes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>

// Max decimal digits in uintmax_t. Used for allocating itoa char buffers.
#if CHAR_BIT == 8
#define MAX_DIGITS (sizeof(uintmax_t) * 3)
#else // what kind of crazy system are you running where char is not 8 bits??
#define MAX_DIGITS ((CHAR_BIT * sizeof(uintmax_t) * 3) / 8)
#endif//

int pf_vprintf(
    const char fmt[restrict static 1], va_list args);
int pf_vfprintf(
    FILE stream[restrict static 1], const char fmt[restrict static 1], va_list args);
int pf_vsprintf(
    char buf[restrict static 1], const char fmt[restrict static 1], va_list args);
int pf_vsnprintf(
    char* restrict buf, size_t, const char fmt[restrict static 1], va_list args);

__attribute__((format (printf, 1, 2)))
int pf_printf(
    const char fmt[restrict static 1], ...);

__attribute__((format (printf, 2, 3)))
int pf_fprintf(
    FILE stream[restrict static 1], const char fmt[restrict static 1], ...);

__attribute__((format (printf, 2, 3)))
int pf_sprintf(char buf[restrict static 1], const char fmt[restrict static 1], ...);

__attribute__((format (printf, 3, 4)))
int pf_snprintf(
    char* restrict buf, size_t, const char fmt[restrict static 1], ...);

#endif // PRINTF_H_INCLUDED

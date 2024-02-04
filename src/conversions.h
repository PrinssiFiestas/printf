#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED 1

#include <printf/printf.h>

// Returns number of characters written excluding null-terminator. Does not
// write more than n characters.
unsigned pf_utoa(size_t n, char* buf, uintmax_t x);
unsigned pf_otoa(size_t n, char* buf, uintmax_t x);
unsigned pf_xtoa(size_t n, char* buf, uintmax_t x);
unsigned pf_Xtoa(size_t n, char* buf, uintmax_t x);
unsigned pf_itoa(size_t n, char* buf, intmax_t x);

#endif // CONVERSIONS_H_INCLUDED

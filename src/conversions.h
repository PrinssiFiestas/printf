#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED 1

#include <printf/printf.h>

// Returns number of characters written excluding null-terminator
unsigned pf_utoa(char buf[static MAX_DIGITS], uintmax_t x);
unsigned pf_otoa(char buf[static MAX_DIGITS], uintmax_t x);
unsigned pf_xtoa(char buf[static MAX_DIGITS], uintmax_t x);
unsigned pf_Xtoa(char buf[static MAX_DIGITS], uintmax_t x);
unsigned pf_itoa(char buf[static MAX_DIGITS], intmax_t x);

#endif // CONVERSIONS_H_INCLUDED

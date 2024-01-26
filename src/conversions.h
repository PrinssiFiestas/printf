#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED 1

#include <printf/printf.h>

char* pf_utoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x);
char* pf_itoa(char buf[GP_STATIC MAX_DIGITS], intmax_t x);
char* pf_otoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x);
char* pf_xtoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x);

#endif // CONVERSIONS_H_INCLUDED

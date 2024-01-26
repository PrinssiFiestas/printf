#include "conversions.h"

static char* str_reverse(size_t len, char buf[GP_STATIC len])
{
    for (size_t i = 0; i < len/2; i++)
    {
        char temp = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = temp;
    }
    return buf;
}

static char* pf_utoa_light(char* buf, uintmax_t x)
{
    size_t i = 0;
    while (x) // write all digits from low to high
    {
        buf[i++] = (char)(x % 10 + '0');
        x /= 10;
    }
    return str_reverse(i, buf);
}

char* pf_utoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x)
{
    memset(buf, 0, MAX_DIGITS);
    return pf_utoa_light(buf, x);
}

char* pf_itoa(char buf[GP_STATIC MAX_DIGITS], intmax_t x)
{
    memset(buf, 0, MAX_DIGITS);
    if (x < 0)
    {
        buf[0] = '-';
        return pf_utoa_light(buf + 1, (uintmax_t)-x) - 1;
    }
    else return pf_utoa_light(buf, (uintmax_t)x);
}

static char* pf_otoa_light(char* buf, uintmax_t x)
{
    size_t i = 0;
    while (x) // write all digits from low to high
    {
        buf[i++] = (char)(x % 8 + '0');
        x /= 8;
    }
    return str_reverse(i, buf);
}

char* pf_otoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x)
{
    memset(buf, 0, MAX_DIGITS);
    return pf_otoa_light(buf, x);
}

static char* pf_xtoa_light(char* buf, uintmax_t x)
{
    size_t i = 0;
    while (x) // write all digits from low to high
    {
        char digit = (char)(x % 16);
        buf[i++] = (char)(digit <= 9 ? digit + '0' : digit - 10 + 'a');
        x /= 16;
    }
    return str_reverse(i, buf);
}

char* pf_xtoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x)
{
    memset(buf, 0, MAX_DIGITS);
    return pf_xtoa_light(buf, x);
}

// In some systems floats can have different endiannes than system endiannes.
static bool little_endian_double(void)
{
    double f = -0.0;
    unsigned char* p = (unsigned char*)&f;
    return p[0] == 0;
}

// TODO implement. For now, we'll settle with strfromf()
char* pf_ftoa(char buf[GP_STATIC MAX_DIGITS], double f)
{
    (void)f;
    (void)little_endian_double();
    return buf;
}

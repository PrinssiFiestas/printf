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

unsigned pf_utoa(char* buf, uintmax_t x)
{
    size_t i = 0;
    do // write all digits from low to high
    {
        buf[i++] = x % 10 + '0';
        x /= 10;
    } while(x);

    buf[i] = '\0';
    str_reverse(i, buf);
    return i;
}

unsigned pf_itoa(char buf[GP_STATIC MAX_DIGITS], intmax_t x)
{
    if (x < 0)
    {
        buf[0] = '-';
        return pf_utoa(buf + 1, -x) + 1;
    }
    else return pf_utoa(buf, x);
}

unsigned pf_otoa(char* buf, uintmax_t x)
{
    size_t i = 0;
    do // write all digits from low to high
    {
        buf[i++] = x % 8 + '0';
        x /= 8;
    } while(x);

    buf[i] = '\0';
    str_reverse(i, buf);
    return i;
}

unsigned pf_xtoa(char* buf, uintmax_t x)
{
    size_t i = 0;
    do // write all digits from low to high
    {
        char digit = x % 16;
        buf[i++] = digit <= 9 ? digit + '0' : digit - 10 + 'a';
        x /= 16;
    } while (x);

    buf[i] = '\0';
    str_reverse(i, buf);
    return i;
}

unsigned pf_Xtoa(char* buf, uintmax_t x)
{
    size_t i = 0;
    do // write all digits from low to high
    {
        char digit = x % 16;
        buf[i++] = digit <= 9 ? digit + '0' : digit - 10 + 'A';
        x /= 16;
    } while (x);

    buf[i] = '\0';
    str_reverse(i, buf);
    return i;
}

// ---------------------------------------------------------------------------

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

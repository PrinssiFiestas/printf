#include <printf/printf.h>

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
        buf[i++] = (char)(x % 10) + '0';
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
        buf[i++] = (char)(x % 8) + '0';
        x /= 8;
    }
    return str_reverse(i, buf);
}

char* pf_otoa(char buf[GP_STATIC MAX_DIGITS], uintmax_t x)
{
    memset(buf, 0, MAX_DIGITS);
    return pf_otoa_light(buf, x);
}

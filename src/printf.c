#include <printf/printf.h>
#include "format_parsing.h"
#include "conversions.h"

static uintmax_t get_uint(va_list args[static 1], const PFFormatSpecifier fmt)
{
    switch (fmt.length_modifier)
    {
        case 'l':
            return va_arg(*args, long);

        case 2 * 'l':
            return va_arg(*args, long long);

        case 'z':
            return va_arg(*args, size_t);

        case 'j':
            return va_arg(*args, uintmax_t);

        default: // rely on integer promotion
            return va_arg(*args, int);
    }
}

static unsigned pad_zeroes(
    char out_buf[static 1],
    const PFFormatSpecifier fmt,
    unsigned written)
{
    if (fmt.precision.option != PF_NONE)
    {
        if (written < fmt.precision.width)
        {
            int diff = fmt.precision.width - written;

            // Make room for zeroes
            memmove(out_buf + diff, out_buf, written);

            // Write zeroes
            while (diff > 0)
            {
                const char* zeroes = "0000""0000""0000""0000";
                const int zeroes_len = (int)strlen(zeroes);
                const int _max = diff >= zeroes_len ?
                    zeroes_len : diff;

                memcpy(out_buf, zeroes, _max);
                diff    -= zeroes_len;
                out_buf += zeroes_len;
            }
            written = fmt.precision.width;
        }
    }
    return written;
}

static unsigned write_o(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const uintmax_t u = get_uint(args, fmt);
    const unsigned written = pf_otoa(out_buf, u);

    if (fmt.flag.hash && u > 0)
    {
        memmove(out_buf + strlen("0"), out_buf, written);
        memcpy(out_buf, "0", strlen("0"));
        return strlen("0") + pad_zeroes(
            out_buf + strlen("0"), fmt, written);
    }
    else
        return pad_zeroes(out_buf, fmt, written);
}

static unsigned write_x(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const uintmax_t u = get_uint(args, fmt);
    const unsigned written = pf_xtoa(out_buf, u);

    if (fmt.flag.hash && u > 0)
    {
        memmove(out_buf + strlen("0x"), out_buf, written);
        memcpy(out_buf, "0x", strlen("0x"));
        return strlen("0x") + pad_zeroes(
            out_buf + strlen("0x"), fmt, written);
    }
    else
        return pad_zeroes(out_buf, fmt, written);
}

static unsigned write_X(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const uintmax_t u = get_uint(args, fmt);
    const unsigned written = pf_Xtoa(out_buf, u);

    if (fmt.flag.hash && u > 0)
    {
        memmove(out_buf + strlen("0X"), out_buf, written);
        memcpy(out_buf, "0X", strlen("0X"));
        return strlen("0X") + pad_zeroes(
            out_buf + strlen("0X"), fmt, written);
    }
    else
        return pad_zeroes(out_buf, fmt, written);
}

static unsigned write_u(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    uintmax_t u = get_uint(args, fmt);
    const unsigned written = pf_utoa(out_buf, u);
    return pad_zeroes(out_buf, fmt, written);
}

// ---------------------------------------------------------------------------

int pf_vsprintf(
    char out_buf[static 1],
    const char format[static 1],
    va_list args)
{
    unsigned chars_written = 0;

    #define update_counters(U) do \
    { \
        unsigned _u = U; \
        chars_written += _u; \
        out_buf += _u; \
    } while (0)

    // TODO put this in a loop
    {
        PFFormatSpecifier fmt = parse_format_string(format, &args);
        strncpy(out_buf, format, fmt.string - format);
        update_counters(fmt.string - format);

        format = fmt.string + fmt.string_length;

        switch (fmt.conversion_format)
        {
            case 'c':
                *out_buf = (char)va_arg(args, int);
                update_counters(1);
                break;

            case 's':
            {
                const char* cstr = va_arg(args, const char*);
                size_t cstr_len;
                if (fmt.precision.option != PF_NONE)
                {
                    const int width = fmt.precision.option == PF_ASTERISK ?
                        va_arg(args, int) : fmt.precision.width;

                    if (width >= 0)
                        cstr_len = width;
                    else
                        cstr_len = strlen(cstr);
                }
                else
                {
                    cstr_len = strlen(cstr);
                }
                memcpy(out_buf, cstr, cstr_len);
                update_counters(cstr_len);
                break;
            }

            case 'd':
            case 'i':
            {
                intmax_t i;
                switch (fmt.length_modifier)
                {
                    case 'l':
                        i = va_arg(args, long);
                        break;

                    case 2 * 'l':
                        i = va_arg(args, long long);
                        break;

                    case 't':
                        i = va_arg(args, ptrdiff_t);
                        break;

                    case 'j':
                        i = va_arg(args, intmax_t);
                        break;

                    default: // rely on integer promotion
                        i = va_arg(args, int);
                }


                unsigned i_written = pf_itoa(out_buf, i);

                if (i < 0)
                {
                    i_written = strlen("-") + pad_zeroes(
                        out_buf + strlen("-"), fmt,
                        i_written - strlen("-"));
                }
                else
                    i_written = pad_zeroes(out_buf, fmt, i_written);

                update_counters(i_written);
                break;
            }

            case 'o':
                update_counters(write_o(out_buf, &args, fmt));
                break;

            case 'x':
                update_counters(write_x(out_buf, &args, fmt));
                break;

            case 'X':
                update_counters(write_X(out_buf, &args, fmt));
                break;

            case 'u':
                update_counters(write_u(out_buf, &args, fmt));
                break;

            case 'p': // lazy solution but works
            {
                uintmax_t u = va_arg(args, uintptr_t);
                update_counters(
                    pf_sprintf(out_buf, "%#jx", u));
                break;
            }

            case 'f': case 'F':
            case 'e': case 'E':
            case 'a': case 'A':
            case 'g': case 'G':
            {
                // TODO actually simplify
                char simplified_fmt[sizeof("%.llf") + MAX_DIGITS] = "";
                strcpy(simplified_fmt, fmt.string);

                char lmod = fmt.length_modifier;
                if (lmod == 'L' || lmod == 'l' || lmod == 2 * 'l')
                    update_counters(strfroml(
                        out_buf, SIZE_MAX / 2 - 1, simplified_fmt,
                        va_arg(args, long double)));
                else
                    update_counters(strfromd(
                        out_buf, SIZE_MAX / 2 - 1, simplified_fmt,
                        va_arg(args, double)));
                break;
            }
        }
    }

    // Write what's left in format string
    strcpy(out_buf, format);
    chars_written += strlen(format);

    return chars_written;
    #undef update_counters
}

__attribute__ ((format (printf, 2, 3)))
int pf_sprintf(char buf[static 1], const char fmt[static 1], ...)
{
    va_list args;
    va_start(args, fmt);
    unsigned written = pf_vsprintf(buf, fmt, args);
    va_end(args);
    return written;
}


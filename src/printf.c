#include <printf/printf.h>
#include "format_parsing.h"
#include "conversions.h"

static uintmax_t get_uint(va_list* args, const PFFormatSpecifier fmt)
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


unsigned pf_vsprintf(
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
        PFFormatSpecifier fmt = parse_format_string(format);
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
                size_t cstr_len = strlen(cstr);
                memcpy(out_buf, cstr, cstr_len + sizeof(""));
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
                update_counters(pf_itoa(out_buf, i));
                break;
            }

            case 'o':
                update_counters(pf_otoa(out_buf, get_uint(&args, fmt)));
                break;

            case 'p':
            case 'x':
                update_counters(pf_xtoa(out_buf, get_uint(&args, fmt)));
                break;

            case 'X':
                update_counters(pf_Xtoa(out_buf, get_uint(&args, fmt)));
                break;

            case 'u':
                update_counters(pf_utoa(out_buf, get_uint(&args, fmt)));
                break;

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
unsigned pf_sprintf(char buf[static 1], const char fmt[static 1], ...)
{
    va_list args;
    va_start(args, fmt);
    unsigned written = pf_vsprintf(buf, fmt, args);
    va_end(args);
    return written;
}


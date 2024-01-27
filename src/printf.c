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

    // TODO put this in a loop
    {
        PFFormatSpecifier fmt = parse_format_string(format);
        strncpy(out_buf, format, fmt.string - format);

        out_buf += fmt.string - format;
        format = fmt.string + fmt.string_length;

        switch (fmt.conversion_format)
        {
            case 'c':
                *out_buf = (char)va_arg(args, int);
                out_buf++;
                break;

            case 's':
            {
                const char* cstr = va_arg(args, const char*);
                size_t cstr_len = strlen(cstr);
                strncpy(out_buf, cstr, cstr_len + sizeof(""));
                out_buf += cstr_len;
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

                    default: // rely on integer promotion
                        i = va_arg(args, int);
                }
                out_buf += pf_itoa(out_buf, i);
                break;
            }

            case 'o':
                out_buf += pf_otoa(out_buf, get_uint(&args, fmt));
                break;

            case 'p':
            case 'x':
                out_buf += pf_xtoa(out_buf, get_uint(&args, fmt));
                break;

            case 'X':
                out_buf += pf_Xtoa(out_buf, get_uint(&args, fmt));
                break;

            case 'u':
                out_buf += pf_utoa(out_buf, get_uint(&args, fmt));
                break;

            case 'f': case 'F':
            case 'e': case 'E':
            case 'a': case 'A':
            case 'g': case 'G':
            {
                char simplified_fmt[32] = ""; // TODO actually simplify
                strcpy(simplified_fmt, fmt.string);
                out_buf += strfromd(out_buf, SIZE_MAX / 2 - 1, simplified_fmt,
                    va_arg(args, double));
                break;
            }

            // TODO
            // default: // INTERNAL ERROR
        }
    }

    // Write what's left in format string
    strcpy(out_buf, format);

    return chars_written;
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


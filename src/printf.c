#include <printf/printf.h>
#include "format_parsing.h"
#include "conversions.h"

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
                out_buf += pf_itoa(out_buf, va_arg(args, int));
                break;

            case 'o':
                out_buf += pf_otoa(out_buf, va_arg(args, unsigned));
                break;

            case 'x':
                out_buf += pf_xtoa(out_buf, va_arg(args, unsigned));
                break;

            case 'X':
                out_buf += pf_Xtoa(out_buf, va_arg(args, unsigned));
                break;

            case 'u':
                out_buf += pf_utoa(out_buf, va_arg(args, unsigned));
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

            case 'p':
                break;

            // TODO
            // default: // INTERNAL ERROR
        }
    }

    // Write what's left in format string
    strcpy(out_buf, format);

    return chars_written;
}

unsigned pf_sprintf(char buf[static 1], const char fmt[static 1], ...)
{
    va_list args;
    va_start(args, fmt);
    unsigned written = pf_vsprintf(buf, fmt, args);
    va_end(args);
    return written;
}

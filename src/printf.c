#include <printf/printf.h>
#include "format_parsing.h"

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
                *out_buf = va_arg(args, int);
                out_buf++;
                break;

            case 's':
                break;

            case 'd':
            case 'i':
                break;

            case 'o':
                break;

            case 'x':
            case 'X':
                break;

            case 'u':
                break;

            case 'f':
            case 'F':
                break;

            case 'e':
            case 'E':
                break;

            case 'a':
            case 'A':
                break;

            case 'g':
            case 'G':
                break;

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


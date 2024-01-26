#include "format_parsing.h"

PFFormatSpecifier
parse_format_string(const char fmt_string[GP_STATIC sizeof("%i")])
{
    PFFormatSpecifier fmt = { fmt_string };

    // Find first format specifier in format string ignoring "%%"
    {
        while ((fmt.string = strchr(fmt.string, '%')) != NULL
                && fmt.string[1] == '%') {
            fmt.string += strlen("%%");
    }
    if (fmt.string == NULL)
            return fmt;
    }

    // Iterator
    const char* c = fmt.string + strlen("%");

    // Find all flags if any
    for (const char* flag; (flag = strchr("-+ #0", *c)); c++)
    {
        switch (*flag)
        {
            case '-': fmt.flag.dash  = 1; break;
            case '+': fmt.flag.plus  = 1; break;
            case ' ': fmt.flag.space = 1; break;
            case '#': fmt.flag.hash  = 1; break;
            case '0': fmt.flag.zero  = 1; break;
        }
    }

    // Find field width
    {
        if (*c == '*')
        {
            fmt.field.asterisk = true;
            c++;
        }
        else if ('1' <= *c && *c <= '9') // can't be 0. Leading 0 is a flag.
        {
            const char* num = c;
            unsigned digits = 0;
            do {
                digits++;
                c++;
            } while ('0' <= *c && *c <= '9');

            unsigned digit = 1;
            while (digits)
            {
                fmt.field.width += (num[digits - 1] - '0') * digit;
                digit *= 10;
                digits--;
            }
        }
    }

    // Find precision
    if (*c == '.')
    {
        c++; // ignore '.'

        if (*c == '*')
        {
            fmt.precision.asterisk = true;
            c++;
        }
        else
        {
            const char* num = c;
            unsigned digits = 0;
            do {
                digits++;
                c++;
            } while ('0' <= *c && *c <= '9');

            unsigned digit = 1;
            while (digits)
            {
                fmt.precision.width += (num[digits - 1] - '0') * digit;
                digit *= 10;
                digits--;
            }
        }
    }

    // Find length modifier
    const char* modifier = strchr("hljztL", *c);
    if (modifier != NULL)
    {
        fmt.length_modifier = *modifier;
        c++;
        if (*modifier == 'h' && *c == 'h') {
            fmt.length_modifier += 'h';
            c++;
        }
        if (*modifier == 'l' && *c == 'l') {
            fmt.length_modifier += 'l';
            c++;
        }
    }

    fmt.conversion_format = *c;
    fmt.string_length = (c + 1/*get to end of string*/) - fmt.string;

    return fmt;
}

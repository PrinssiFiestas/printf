#include <printf/printf.h>

typedef struct PFFormatSpecifier
{
    const char* string;     // Pointer to '%', NULL if no format specifier.
    unsigned string_length; // e.g. 4 for "%.3f"

    union
    {
        unsigned any;
        struct // TODO some lookuptableythingy
        {
            bool dash;
            bool plus;
            bool space;
            bool hash;
            bool zero;
        };
    } flag;
    struct
    {
        unsigned width;
        bool asterisk;
    } field;
    struct
    {
        unsigned width;
        bool asterisk;
    } precision;
    char length_modifier;   // any of "hljztL" or 2*'h' or 2*'l'
    char conversion_format; // any of "csdioxXufFeEaAgGnP"
} PFFormatSpecifier;

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
        switch (*flag) // TODO some lookuptableythingy
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
                fmt.field.width += num[digits - 1] - '0' * digit;
                digit *= 10;
                digits--;
            }
        }
    }

    // Find precision
    if (*c == '.')
    {
        c++;

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
                fmt.precision.width += num[digits - 1] - '0' * digit;
                digit *= 10;
                digits--;
            }
        }
    }

    // Fund length modifier
    const char* modifier = strchr("hljztL", *c);
    if (modifier != NULL)
    {
        fmt.length_modifier = *modifier;
        c++;
        if (*modifier == 'h' && *c == 'h') {
            modifier += 'h';
            c++;
        }
        if (*modifier == 'l' && *c == 'l') {
            modifier += 'l';
            c++;
        }
    }

    fmt.conversion_format = *c;
    fmt.string_length = (size_t)(c - fmt.string);

    return fmt;
}

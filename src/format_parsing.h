#ifndef FORMAT_PARSING_H_INCLUDED
#define FORMAT_PARSING_H_INCLUDED 1

#include <printf/printf.h>

typedef struct PFFormatSpecifier
{
    const char* string;     // Pointer to '%', NULL if no format specifier.
    unsigned string_length; // e.g. 4 for "%.3f"

    union // flag
    {
        unsigned any;
        struct
        {
            bool dash;
            bool plus;
            bool space;
            bool hash;
            bool zero;
        };
    } flag;

    struct // field
    {
        unsigned width;
        bool asterisk;
    } field;

    struct // precision
    {
        unsigned width;
        enum // option
        {
            PF_NONE,
            PF_SOME,
            PF_ASTERISK
        } option;
    } precision;

    char length_modifier;   // any of "hljztL" or 2*'h' or 2*'l'
    char conversion_format; // any of "csdioxXufFeEaAgGp". 'n' not supported.
} PFFormatSpecifier;

PFFormatSpecifier
parse_format_string(const char fmt_string[GP_STATIC sizeof("%i")]);

#endif // FORMAT_PARSING_H_INCLUDED

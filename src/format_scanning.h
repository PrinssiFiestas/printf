#ifndef FORMAT_SCANNING_H_INCLUDED
#define FORMAT_SCANNING_H_INCLUDED 1

#include <printf/printf.h>

typedef struct PFFormatSpecifier
{
    const char* string;     // Pointer to '%', NULL if no format specifier.
    unsigned string_length; // e.g. 4 for "%.3f"

    struct // flag
    {
        bool dash;
        bool plus;
        bool space;
        bool hash;
        bool zero;
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

    unsigned char length_modifier;   // any of "hljztL" or 2*'h' or 2*'l'
    unsigned char conversion_format; // any of "csdioxXufFeEaAgGp". 'n' not supported.
} PFFormatSpecifier;

// Portability wrapper.
// https://stackoverflow.com/questions/8047362/is-gcc-mishandling-a-pointer-to-a-va-list-passed-to-a-function
typedef struct pf_va_list
{
    va_list list;
} pf_va_list;

PFFormatSpecifier
scan_format_string(
    const char fmt_string[GP_STATIC sizeof("%i")],
    pf_va_list* asterisks); // optional

#endif // FORMAT_SCANNING_H_INCLUDED

#include <printf/printf.h>
#include "format_parsing.h"
#include "conversions.h"

static uintmax_t get_uint(va_list args[static 1], const PFFormatSpecifier fmt)
{
    switch (fmt.length_modifier)
    {
        case 'l':
            return va_arg(*args, unsigned long);

        case 2 * 'l':
            return va_arg(*args, unsigned long long);

        case 'z':
            return va_arg(*args, size_t);

        case 'j':
            return va_arg(*args, uintmax_t);

        default: // rely on integer promotion
            return va_arg(*args, unsigned);
    }
}

static unsigned pad_zeroes(
    char out_buf[static 1],
    const PFFormatSpecifier fmt,
    unsigned written)
{
    if (fmt.precision.option != PF_NONE && written < fmt.precision.width)
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
    return written;
}

static inline void
progress(char* out_buf[static 1], unsigned written[static 1],unsigned u)
{
    *written += u;
    *out_buf += u;
};

static unsigned write_s(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    unsigned written = 0;
    const char* cstr = va_arg(*args, const char*);
    if (cstr == NULL)
    {
        if (fmt.precision.option == PF_SOME &&
            fmt.precision.width < strlen("(null)"))
            cstr = "";
        else
            cstr = "(null)";
    }

    size_t cstr_len = 0;
    if (fmt.precision.option == PF_NONE) // should be null-terminated
        cstr_len = strlen(cstr);
    else // who knows if null-terminated
        while (cstr_len < fmt.precision.width && cstr[cstr_len] != '\0')
            cstr_len++;

    const unsigned field_width = fmt.field.width > cstr_len ?
        fmt.field.width : cstr_len;
    const unsigned diff = field_width - cstr_len;
    if (fmt.flag.dash) // left justified
    { // first string, then pad
        memcpy(out_buf, cstr, cstr_len);
        progress(&out_buf, &written, cstr_len);
        for (unsigned i = 0; i < diff; i++)
            out_buf[i] = ' ';
        progress(&out_buf, &written, diff);
    }
    else // first pad, then string
    {
        for (unsigned i = 0; i < diff; i++)
            out_buf[i] = ' ';
        progress(&out_buf, &written, diff);
        memcpy(out_buf, cstr, cstr_len);
        progress(&out_buf, &written, cstr_len);
    }

    return written;
}
static unsigned write_i(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    intmax_t i;
    switch (fmt.length_modifier)
    {
        case 'l':
            i = va_arg(*args, long);
            break;

        case 2 * 'l':
            i = va_arg(*args, long long);
            break;

        case 't':
            i = va_arg(*args, ptrdiff_t);
            break;

        case 'j':
            i = va_arg(*args, intmax_t);
            break;

        default: // rely on integer promotion
            i = va_arg(*args, int);
    }

    unsigned i_written = pf_itoa(out_buf, i);

    if (i < 0)
    {
        return strlen("-") + pad_zeroes(
            out_buf + strlen("-"), fmt,
            i_written - strlen("-"));
    }
    else if (fmt.flag.plus || fmt.flag.space)
    {
        memmove(out_buf + strlen("+"), out_buf, i_written); // make room for plus
        out_buf[0] = fmt.flag.plus ? '+' : ' ';

        return strlen("+") + pad_zeroes(
            out_buf + strlen("+"), fmt,
            i_written);
    }
    else
        return pad_zeroes(out_buf, fmt, i_written);
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
        out_buf[0] = '0';

        return pad_zeroes(out_buf, fmt, written + strlen("."));
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

static unsigned write_p(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const uintmax_t u = get_uint(args, fmt);

    if (u > 0)
    {
        strcpy(out_buf, "0x");
        return strlen("0x") + pad_zeroes(
            out_buf + strlen("0x"), fmt, pf_xtoa(out_buf + strlen("0x"), u));
    }
    else
    {
        strcpy(out_buf, "(nil)");
        return strlen("(nil)");
    }
}

static unsigned write_f(
    char out_buf[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    char* const start = out_buf;
    unsigned written = 0;

    // strfromd() doesn't take flags or field width so fmt.string needs to be
    // trimmed.
    char simplified_fmt[sizeof("%.llf") + MAX_DIGITS] = "%";
    const char* ffmt = fmt.string + strcspn(fmt.string, ".aAeEfFgG");
    memcpy(
        simplified_fmt + strlen("%"),
        ffmt,
        fmt.string + fmt.string_length - ffmt);

    char lmod = fmt.length_modifier;
    if (lmod == 'L' || lmod == 'l' || lmod == 2 * 'l')
        progress(&out_buf, &written, strfroml(
            out_buf, SIZE_MAX / 2 - 1, simplified_fmt,
            va_arg(*args, long double)));
    else
        progress(&out_buf, &written, strfromd(
            out_buf, SIZE_MAX / 2 - 1, simplified_fmt,
            va_arg(*args, double)));

    if ((fmt.flag.plus || fmt.flag.space) && start[0] != '-')
    { // write plus or space if positive
        memmove(start + strlen("+"), start, written);
        start[0] = fmt.flag.plus ? '+' : ' ';
        progress(&out_buf, &written, strlen("+"));
    }

    if (fmt.flag.hash)
    {
        char* decimal_point = memchr(start, '.', written);
        const bool is_whole_num = ! decimal_point;
        const bool has_sign = strchr("+ -", start[0]) != NULL;
        char* exponent = memchr(start, 'e', written);
        if ( ! exponent) // try again
            exponent = memchr(start, 'E', written);
        const bool is_nan_or_inf =
            start[has_sign] == 'n' || start[has_sign] == 'N' || // "nan"
            start[has_sign] == 'i' || start[has_sign] == 'I';   // "inf"

        if ( ! decimal_point && ! is_nan_or_inf) // write point
        {
            if (exponent)
            {
                memmove(exponent + strlen("."), exponent, out_buf - exponent);
                (decimal_point = exponent)[0] = '.';
                exponent += strlen(".");
            }
            else
            {
                (decimal_point = out_buf)[0] = '.';
            }
            progress(&out_buf, &written, strlen("."));
        }
        out_buf[0] = '\0'; // for strspn()

        if ((fmt.conversion_format == 'g' || fmt.conversion_format == 'G')
            && ! is_nan_or_inf)
        {
            unsigned significant_digits_written = 0;
            if (is_whole_num)
            {
                significant_digits_written =
                    strspn(start + has_sign, "0123456789");
            }
            else if (start[0 + has_sign] == '0' && start[1 + has_sign] == '.')
            { // number is less than 0 and leading zeroes should be ignored
                const char* significant_digits_start = decimal_point + strlen(".");
                while (significant_digits_start[0] == '0')
                    significant_digits_start++;

                significant_digits_written =
                    strspn(significant_digits_start, "0123456789");
            }
            else // non-whole number with absolute value larger than 0
            {
                significant_digits_written =
                    strspn(start + has_sign, ".0123456789") - strlen(".");
            }

            const unsigned precision = fmt.precision.option == PF_SOME ?
                fmt.precision.width :
                6/*default %g precision*/;

            if (precision > significant_digits_written) // write trailing zeroes
            {
                const unsigned diff = precision - significant_digits_written;
                if (exponent) // make room for zeroes
                    memmove(exponent + diff, exponent, out_buf - exponent);

                char* end_of_digits = decimal_point + strlen(".");
                end_of_digits += strspn(end_of_digits, "0123456789");
                for (size_t i = 0; i < diff; i++)
                    end_of_digits[i] = '0';

                progress(&out_buf, &written, diff);
            }
        }
    }
    return written;
}

// ---------------------------------------------------------------------------

int pf_vsnprintf(
    char out_buf[static 1],
    size_t max_size,
    const char format[static 1],
    va_list args)
{
    (void)max_size;
    unsigned chars_written = 0;

    while (1)
    {
        const PFFormatSpecifier fmt = parse_format_string(format, &args);
        if (fmt.string == NULL)
            break;

        memcpy(out_buf, format, fmt.string - format);
        chars_written += fmt.string - format;
        out_buf       += fmt.string - format;

        format = fmt.string + fmt.string_length;

        // Number of characters written by converting format specifier
        unsigned written = 0;

        switch (fmt.conversion_format)
        {
            case 'c':
                out_buf[0] = (char)va_arg(args, int);
                out_buf[1] = '\0';
                progress(&out_buf, &written, strlen("x"));
                break;

            case 's':
                progress(&out_buf, &written, write_s(out_buf, &args, fmt));
                break;

            case 'd':
            case 'i':
                progress(&out_buf, &written, write_i(out_buf, &args, fmt));
                break;

            case 'o':
                progress(&out_buf, &written, write_o(out_buf, &args, fmt));
                break;

            case 'x':
                progress(&out_buf, &written, write_x(out_buf, &args, fmt));
                break;

            case 'X':
                progress(&out_buf, &written, write_X(out_buf, &args, fmt));
                break;

            case 'u':
                progress(&out_buf, &written, write_u(out_buf, &args, fmt));
                break;

            case 'p':
                progress(&out_buf, &written, write_p(out_buf, &args, fmt));
                break;

            case 'f': case 'F':
            case 'e': case 'E':
            case 'a': case 'A':
            case 'g': case 'G':
                progress(&out_buf, &written, write_f(out_buf, &args, fmt));
                break;

            case '%':
                out_buf[0] = '%';
                out_buf[1] = '\0';
                progress(&out_buf, &written, strlen("%"));
                break;
        }

        if (written < fmt.field.width) // add padding
        {
            char* const start = out_buf - written;
            const unsigned diff = fmt.field.width - written;
            const bool has_sign =
                start[0] == '-' ||
                start[0] == '+' ||
                start[0] == ' '; // space flag: ' ' <=> '+'

            const bool ignore_zero =
            (
                strchr("diouxX", fmt.conversion_format) != NULL &&
                fmt.precision.option != PF_NONE
            ) || (
                start[has_sign] == 'n' || start[has_sign] == 'N' || // "nan"
                start[has_sign] == 'i' || start[has_sign] == 'I'    // "inf"
            );

            if (fmt.flag.dash) // left justified, append padding
            {
                for (size_t i = 0; i < diff; i++)
                    out_buf[i] = ' ';
            }
            else if (fmt.flag.zero && ! ignore_zero) // fill in zeroes
            { // 0-padding minding "0x" or sign prefix
                const bool has_0x =
                    start[1 + has_sign] == 'x' ||
                    start[1 + has_sign] == 'X';
                const unsigned offset = has_sign + 2 * has_0x;

                // Make foom for zeroes
                memmove(
                // printf("%#07x", 0x89)
                //
                // 0x89___
                //      x    <- dest == start + diff + offset
                //   x       <- src  == start + offset
                //   89      <- offset == strlen("0x") == 2 so these go
                //      89   <- here
                    start + diff + offset,
                    start + offset,
                    written - offset);

                for (size_t i = 0; i < diff; i++)
                    (start + offset)[i] = '0';
            }
            else // fill in spaces
            {
                memmove(start + diff, start, written);
                for (size_t i = 0; i < diff; i++)
                    start[i] = ' ';
            }

            progress(&out_buf, &written, diff);
        }

        chars_written += written;
    }

    // Write what's left in format string
    strcpy(out_buf, format);
    chars_written += strlen(format);

    return chars_written;
}

int pf_vsprintf(char buf[static 1], const char fmt[static 1], va_list args)
{
    // Some implementations of snprintf() and strfromd() don't accept sizes
    // larger than SIZE_MAX/2. Also the return value which is written
    // characters is int and INT_MAX < SIZE_MAX in most systems. This means that
    // correctness was never achievable in the first place so let's just settle
    // with INT_MAX.
    return pf_vsnprintf(buf, INT_MAX, fmt, args);
}

__attribute__ ((format (printf, 2, 3)))
int pf_sprintf(char buf[static 1], const char fmt[static 1], ...)
{
    va_list args;
    va_start(args, fmt);
    int written = pf_vsnprintf(buf, INT_MAX, fmt, args);
    va_end(args);
    return written;
}

__attribute__ ((format (printf, 3, 4)))
int pf_snprintf(char buf[static 1], size_t n, const char fmt[static 1], ...)
{
    va_list args;
    va_start(args, fmt);
    int written = pf_vsnprintf(buf, n, fmt, args);
    va_end(args);
    return written;
}

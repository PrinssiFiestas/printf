#include <printf/printf.h>
#include "format_scanning.h"
#include "conversions.h"
#include "pfstring.h"
#include <inttypes.h>
#include <math.h>
#include <ctype.h>

// All write_*() family of functions returns what would be written without
// limit, not what is actually written, just like snprintf(NULL, 0, ...).

struct MiscData
{
    bool has_sign;
    bool has_0x;
    bool is_nan_or_inf;
};

static uintmax_t get_uint(va_list args[static 1], const PFFormatSpecifier fmt)
{
    if (fmt.conversion_format == 'p')
        return va_arg(*args, uintptr_t);

    switch (fmt.length_modifier)
    {
        case 'j':
            return va_arg(*args, uintmax_t);

        case 'l' * 2:
            return va_arg(*args, unsigned long long);

        case 'l':
            return va_arg(*args, unsigned long);

        case 'h':
            return (unsigned short)va_arg(*args, unsigned);

        case 'h' * 2:
            return (unsigned char)va_arg(*args, unsigned);

        case 'z':
            return (size_t)va_arg(*args, size_t);

        default:
            return va_arg(*args, unsigned);
    }
}

static unsigned write_s(
    struct PFString* out,
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
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
        concat(out, cstr, cstr_len);
        pad(out, ' ', diff);
    }
    else // first pad, then string
    {
        pad(out, ' ', diff);
        concat(out, cstr, cstr_len);
    }

    return out->length - original_length;
}

static void write_leading_zeroes(
    struct PFString out[static 1],
    const unsigned written_by_utoa,
    const PFFormatSpecifier fmt)
{
    if (fmt.precision.option != PF_NONE)
    {
        const unsigned diff =
            fmt.precision.width <= written_by_utoa ? 0 :
            fmt.precision.width - written_by_utoa;
        memmove(
            out->data + out->length + diff,
            out->data + out->length,
            limit(*out, written_by_utoa));
        memset(out->data + out->length, '0', limit(*out, diff));
        out->length += written_by_utoa + diff;
    }
    else
    {
        out->length += written_by_utoa;
    }
}

static unsigned write_i(
    struct PFString out[static 1],
    struct MiscData md[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    intmax_t i;
    switch (fmt.length_modifier)
    {
        case 'j':
            i = va_arg(*args, intmax_t);
            break;

        case 'l' * 2:
            i = va_arg(*args, long long);
            break;

        case 'l':
            i = va_arg(*args, long);
            break;

        case 'h':
            i = (short)va_arg(*args, int);
            break;

        case 'h' * 2: // signed char is NOT char!
            i = (signed char)va_arg(*args, int);
            break;

        case 't':
            i = (ptrdiff_t)va_arg(*args, ptrdiff_t);
            break;

        default:
            i = va_arg(*args, int);
    }

    const size_t original_length = out->length;

    const char sign = i < 0 ? '-' : fmt.flag.plus ? '+' : fmt.flag.space ? ' ' : 0;
    if (sign)
    {
        push_char(out, sign);
        md->has_sign = true;
    }

    const unsigned max_written = pf_utoa(
        capacity_left(*out), out->data + out->length, imaxabs(i));

    write_leading_zeroes(out, max_written, fmt);
    return out->length - original_length;
}

static unsigned write_o(
    struct PFString out[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
    const uintmax_t u = get_uint(args, fmt);

    bool zero_written = false;
    if (fmt.flag.hash && u > 0)
    {
        push_char(out, '0');
        zero_written = true;
    }

    const unsigned max_written = pf_otoa(
        capacity_left(*out), out->data + out->length, u);

    // zero_written tells pad_zeroes() to add 1 less '0'
    write_leading_zeroes(out, zero_written + max_written, fmt);
    // compensate for added zero_written to write_leading_zeroes()
    out->length -= zero_written;

    return out->length - original_length;
}

static unsigned write_x(
    struct PFString out[static 1],
    struct MiscData md[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
    const uintmax_t u = get_uint(args, fmt);

    if (fmt.flag.hash && u > 0)
    {
        concat(out, "0x", strlen("0x"));
        md->has_0x = true;
    }

    const unsigned max_written = pf_xtoa(
        capacity_left(*out), out->data + out->length, u);

    write_leading_zeroes(out, max_written, fmt);
    return out->length - original_length;
}

static unsigned write_X(
    struct PFString out[static 1],
    struct MiscData md[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
    const uintmax_t u = get_uint(args, fmt);

    if (fmt.flag.hash && u > 0)
    {
        concat(out, "0X", strlen("0X"));
        md->has_0x = true;
    }

    const unsigned max_written = pf_Xtoa(
        capacity_left(*out), out->data + out->length, u);

    write_leading_zeroes(out, max_written, fmt);
    return out->length - original_length;
}

static unsigned write_u(
    struct PFString out[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
    const uintmax_t u = get_uint(args, fmt);
    const unsigned max_written = pf_utoa(
        capacity_left(*out), out->data + out->length, u);
    write_leading_zeroes(out, max_written, fmt);
    return out->length - original_length;
}

static unsigned write_p(
    struct PFString out[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    const size_t original_length = out->length;
    const uintmax_t u = get_uint(args, fmt);

    if (u > 0)
    {
        concat(out, "0x", strlen("0x"));
        const unsigned max_written = pf_xtoa(
            capacity_left(*out), out->data + out->length, u);
        write_leading_zeroes(out, max_written, fmt);
    }
    else
    {
        concat(out, "(nil)", strlen("(nil)"));
    }
    return out->length - original_length;
}

static inline void
progress(char* out_buf[static 1], unsigned written[static 1], unsigned u)
{
    *written += u;
    *out_buf += u;
};

static unsigned write_f(
    struct PFString out[static 1],
    struct MiscData md[static 1],
    va_list args[static 1],
    const PFFormatSpecifier fmt)
{
    char* const start = out->data + out->length;
    const size_t original_length = out->length;

    // strfromd() doesn't take flags or field width so fmt.string needs to be
    // trimmed.
    char simplified_fmt[sizeof("%.llf") + MAX_DIGITS] = "%";
    const char* ffmt = fmt.string + strcspn(fmt.string, ".aAeEfFgG");
    memcpy(
        simplified_fmt + strlen("%"),
        ffmt,
        fmt.string + fmt.string_length - ffmt);

    double f = va_arg(*args, double);

    if (!signbit(f) && fmt.flag.plus)
    {
        push_char(out, '+');
        md->has_sign = true;
    }
    else if (!signbit(f) && fmt.flag.space)
    {
        push_char(out, ' ');
        md->has_sign = true;
    }
    else if (signbit(f))
        md->has_sign = true;

    out->length += strfromd(
        out->data + out->length, capacity_left(*out), simplified_fmt, f);

    md->is_nan_or_inf = isnan(f) || isinf(f);
    if ((fmt.conversion_format == 'a' || fmt.conversion_format == 'A') &&
        ! md->is_nan_or_inf)
        md->has_0x = true;

    if (fmt.flag.hash && ! md->is_nan_or_inf)
    {
        // Better not to use math functions due to floating point errors. All we
        // care about is did strfromd() write a decimal point or not.
        //char* decimal_point = memchr(start, '.', written);
        // UNSAFE fix this!
        char* decimal_point = memchr(start, '.', out->length - original_length);
        const bool is_whole_num = ! decimal_point;
        const bool has_sign = strchr("+ -", start[0]) != NULL;
        //char* exponent = memchr(start, 'e', written);
        //UNSAFE fix this!
        char* exponent = memchr(start, 'e', out->length - original_length);
        if ( ! exponent) // try again
            //exponent = memchr(start, 'E', written);
            //UNSAFE fix this!
            exponent = memchr(start, 'E', out->length - original_length);

        if ( ! decimal_point) // write point
        {
            if (exponent)
            {
                // memmove(exponent + strlen("."), exponent, out_buf - exponent);
                // (decimal_point = exponent)[0] = '.';
                // exponent += strlen(".");
                memmove(exponent + strlen("."), exponent,
                    out->data + out->length - exponent - !capacity_left(*out));
                (decimal_point = exponent)[0] = '.';
                exponent += strlen(".");
                out->length++;
            }
            else
            {
                //(decimal_point = out_buf)[0] = '.';
                decimal_point = out->data + out->length; // is this safe??
                push_char(out, '.');
            }
            //progress(&out_buf, &written, strlen("."));

        }
        // OLD IMPLEMENTATION, GET RID OF THESE. Just write directly to out.
        char* out_buf = out->data + out->length; // <---------------------------
        unsigned written = out->length - original_length; // <------------------
        const unsigned TEMP_written_original = written; // <--------------------

        out_buf[0] = '\0'; // for strspn()

        if (fmt.conversion_format == 'g' || fmt.conversion_format == 'G')
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
                // ---------------------------
                //
                // Exponent may be moved out of bounds when writing comma. Here
                // it may cause problems and need to be fixed!
                //
                // --------------------------
                if (exponent) // make room for zeroes
                    memmove(exponent + diff, exponent, out_buf - exponent);

                char* end_of_digits = decimal_point + strlen(".");
                end_of_digits += strspn(end_of_digits, "0123456789");
                // TODO use pad() instead!
                for (size_t i = 0; i < diff; i++)
                    end_of_digits[i] = '0';

                progress(&out_buf, &written, diff);
            }
        }
        out->length += written - TEMP_written_original;
        return written;
    }
    //out->length += written - TEMP_written_original; // TODO REMOVE
    //return written;

    // NEW
    return out->length - original_length;
}

static unsigned add_padding(
    struct PFString out[static 1],
    const unsigned written,
    const struct MiscData md,
    const PFFormatSpecifier fmt)
{
    size_t start = out->length - written;
    const unsigned diff = fmt.field.width - written;

    const bool is_int_with_precision =
        strchr("diouxX", fmt.conversion_format) && fmt.precision.option != PF_NONE;
    const bool ignore_zero = is_int_with_precision || md.is_nan_or_inf;

    if (fmt.flag.dash) // left justified, append padding
    {
        pad(out, ' ', diff);
    }
    else if (fmt.flag.zero && ! ignore_zero) // fill in zeroes
    { // 0-padding minding "0x" or sign prefix
        const unsigned offset = md.has_sign + 2 * md.has_0x;
        insert_pad(out, start + offset, '0', diff);
    }
    else // fill in spaces
    {
        insert_pad(out, start, ' ', diff);
    }

    return diff;
}



// ---------------------------------------------------------------------------
//
//
//
// IMPLEMENTATIONS OF PUBLIC FUNCTIONS
//
//
//
// ---------------------------------------------------------------------------



int pf_vsnprintf(
    char out_buf[static 1],
    const size_t max_size,
    const char format[static 1],
    va_list args)
{
    struct PFString out = { out_buf, .capacity = max_size };

    while (1)
    {
        const PFFormatSpecifier fmt = scan_format_string(format, &args);
        if (fmt.string == NULL)
            break;

        concat(&out, format, fmt.string - format);

        unsigned written_by_conversion = 0;
        struct MiscData misc = {};

        switch (fmt.conversion_format)
        {
            case 'c':
                push_char(&out, (char)va_arg(args, int));
                written_by_conversion = 1;
                break;

            case 's':
                written_by_conversion += write_s(
                    &out, &args, fmt);
                break;

            case 'd':
            case 'i':
                written_by_conversion += write_i(
                    &out, &misc, &args, fmt);
                break;

            case 'o':
                written_by_conversion += write_o(
                    &out, &args, fmt);
                break;

            case 'x':
                written_by_conversion += write_x(
                    &out, &misc, &args, fmt);
                break;

            case 'X':
                written_by_conversion += write_X(
                    &out, &misc, &args, fmt);
                break;

            case 'u':
                written_by_conversion += write_u(
                    &out, &args, fmt);
                break;

            case 'p':
                written_by_conversion += write_p(
                    &out, &args, fmt);
                break;

            case 'f': case 'F':
            case 'e': case 'E':
            case 'a': case 'A':
            case 'g': case 'G':
                if (sizeof(long double) != sizeof(double) && (
                    fmt.length_modifier == 'L' ||
                    fmt.length_modifier == 'l' ||
                    fmt.length_modifier == 'l' * 2))
                    // TODO add switch to disable long double which would get
                    // rid of snprintf() below reducing code size.
                {
                    char fmt_buf[32];
                    memcpy( // to allow null-termination
                        fmt_buf,
                        fmt.string,
                        min(fmt.string_length, sizeof(fmt_buf)-1));
                    fmt_buf[sizeof(fmt_buf) - 1] = '\0';

                    // Only hope to get any portability.
                    out.length += snprintf(
                        out.data + out.length, max_size,
                        fmt_buf, va_arg(args, long double));
                    format = fmt.string + fmt.string_length;
                    continue;
                }
                written_by_conversion += write_f(
                    &out, &misc, &args, fmt);
                break;

            case '%':
                push_char(&out, '%');
                break;
        }

        if (written_by_conversion < fmt.field.width)
            add_padding(
                &out,
                written_by_conversion,
                misc,
                fmt);

        // Jump over format specifier
        format = fmt.string + fmt.string_length;
    }

    // Write what's left in format string
    size_t tail_length = strlen(format);
    memcpy(out.data + out.length, format, tail_length);
    out.length += tail_length;
    out.data[out.length] = '\0';

    return out.length;
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

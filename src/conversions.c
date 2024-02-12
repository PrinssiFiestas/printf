#include "conversions.h"
#include "format_scanning.h"
#include "d2s_full_table.h"
#include <inttypes.h>
#include <math.h>

#include "ryu.h"
#include "common.h"
#include "digit_table.h"
#include "d2fixed_full_table.h"
#include "d2s_intrinsics.h"

#define DOUBLE_MANTISSA_BITS 52
#define DOUBLE_EXPONENT_BITS 11
#define DOUBLE_BIAS 1023

#define POW10_ADDITIONAL_BITS 120

static void str_reverse_copy(
    char* restrict out,
    char* restrict buf,
    const size_t length,
    const size_t max)
{
    const size_t maxlen = max < length ? max : length;
    for (size_t i = 0; i < maxlen; i++)
        out[i] = buf[length - 1 - i];

    if (length < max)
        out[length] = '\0';
}

static inline void
append_n_digits(const uint32_t olength, uint32_t digits, char* const result);

unsigned pf_utoa(const size_t n, char* out, uintmax_t x)
{
    if (n >= 10 && x < 1000000000) // use optimized
    {
        const uint32_t olength = decimalLength9(x);
        append_n_digits(olength, x, out);
        return olength;
    }

    char buf[MAX_DIGITS];
    size_t i = 0;
    do // write all digits from low to high
    {
        buf[i++] = x % 10 + '0';
        x /= 10;
    } while(x);

    str_reverse_copy(out, buf, i, n);
    return i;
}

unsigned pf_itoa(size_t n, char* out, const intmax_t ix)
{
    char buf[MAX_DIGITS];

    if (ix < 0)
    {
        if (n > 0)
        {
            out[0] = '-';
            n--;
        }
        out++;
    }

    uintmax_t x = imaxabs(ix);
    size_t i = 0;
    do // write all digits from low to high
    {
        buf[i++] = x % 10 + '0';
        x /= 10;
    } while(x);

    str_reverse_copy(out, buf, i, n);
    return i + (ix < 0);
}

unsigned pf_otoa(const size_t n, char* out, uintmax_t x)
{
    char buf[MAX_DIGITS];
    size_t i = 0;
    do // write all digits from low to high
    {
        buf[i++] = x % 8 + '0';
        x /= 8;
    } while(x);

    str_reverse_copy(out, buf, i, n);
    return i;
}

unsigned pf_xtoa(const size_t n, char* out, uintmax_t x)
{
    char buf[MAX_DIGITS];
    size_t i = 0;
    do // write all digits from low to high
    {
        unsigned _x = x % 16;
        buf[i++] = _x < 10 ? _x + '0' : _x - 10 + 'a';
        x /= 16;
    } while(x);

    str_reverse_copy(out, buf, i, n);
    return i;
}

unsigned pf_Xtoa(const size_t n, char* out, uintmax_t x)
{
    char buf[MAX_DIGITS];
    size_t i = 0;
    do // write all digits from low to high
    {
        unsigned _x = x % 16;
        buf[i++] = _x < 10 ? _x + '0' : _x - 10 + 'A';
        x /= 16;
    } while(x);

    str_reverse_copy(out, buf, i, n);
    return i;
}

// ---------------------------------------------------------------------------

static unsigned
pf_d2fixed_buffered_n(
    char* result,
    size_t n,
    PFFormatSpecifier fmt,
    double d);

unsigned
pf_ftoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'f'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned
pf_Ftoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'F'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned
pf_etoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'e'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned
pf_Etoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'E'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned
pf_gtoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'g'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned
pf_Gtoa(const size_t n, char* const buf, const double f)
{
    const PFFormatSpecifier fmt = {.conversion_format = 'G'};
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

unsigned pf_strfromd(
    char* const buf,
    const size_t n,
    const PFFormatSpecifier fmt,
    const double f)
{
    return pf_d2fixed_buffered_n(buf, n, fmt, f);
}

// ---------------------------------------------------------------------------
//
// Modified Ryū
//
// https://dl.acm.org/doi/pdf/10.1145/3192366.3192369
// https://dl.acm.org/doi/pdf/10.1145/3360595
// https://github.com/ulfjack/ryu
//
// ---------------------------------------------------------------------------

// Convert `digits` to a sequence of decimal digits. Append the digits to the
// result.
// The caller has to guarantee that:
//   10^(olength-1) <= digits < 10^olength
// e.g., by passing `olength` as `decimalLength9(digits)`.
static inline void
append_n_digits(const uint32_t olength, uint32_t digits, char* const result)
{
    uint32_t i = 0;
    while (digits >= 10000)
    {
        #ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
            const uint32_t c = digits - 10000 * (digits / 10000);
        #else
            const uint32_t c = digits % 10000;
        #endif
        digits /= 10000;
        const uint32_t c0 = (c % 100) << 1;
        const uint32_t c1 = (c / 100) << 1;
        memcpy(result + olength - i - 2, DIGIT_TABLE + c0, 2);
        memcpy(result + olength - i - 4, DIGIT_TABLE + c1, 2);
        i += 4;
    }
    if (digits >= 100)
    {
        const uint32_t c = (digits % 100) << 1;
        digits /= 100;
        memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
        i += 2;
    }
    if (digits >= 10)
    {
        const uint32_t c = digits << 1;
        memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
    }
    else
    {
        result[0] = (char) ('0' + digits);
    }
}

static inline uint32_t
mulShift_mod1e9(const uint64_t m, const uint64_t* const mul, const int32_t j)
{
    uint64_t high0;                                   // 64
    const uint64_t low0 = umul128(m, mul[0], &high0); // 0
    uint64_t high1;                                   // 128
    const uint64_t low1 = umul128(m, mul[1], &high1); // 64
    uint64_t high2;                                   // 192
    const uint64_t low2 = umul128(m, mul[2], &high2); // 128
    const uint64_t s0low = low0;              // 0
    (void) s0low; // unused
    const uint64_t s0high = low1 + high0;     // 64
    const uint32_t c1 = s0high < low1;
    const uint64_t s1low = low2 + high1 + c1; // 128
    // high1 + c1 can't overflow, so compare against low2
    const uint32_t c2 = s1low < low2;
    const uint64_t s1high = high2 + c2;       // 192
    assert(j >= 128);
    assert(j <= 180);
    #if defined(HAS_64_BIT_INTRINSICS)
        const uint32_t dist = (uint32_t) (j - 128); // dist: [0, 52]
        const uint64_t shiftedhigh = s1high >> dist;
        const uint64_t shiftedlow = shiftright128(s1low, s1high, dist);
        return uint128_mod1e9(shiftedhigh, shiftedlow);
    #else // HAS_64_BIT_INTRINSICS
        if (j < 160)
        { // j: [128, 160)
            const uint64_t r0 = mod1e9(s1high);
            const uint64_t r1 = mod1e9((r0 << 32) | (s1low >> 32));
            const uint64_t r2 = ((r1 << 32) | (s1low & 0xffffffff));
            return mod1e9(r2 >> (j - 128));
        }
        else
        { // j: [160, 192)
            const uint64_t r0 = mod1e9(s1high);
            const uint64_t r1 = ((r0 << 32) | (s1low >> 32));
            return mod1e9(r1 >> (j - 160));
        }
    #endif // HAS_64_BIT_INTRINSICS
}

// Convert `digits` to a sequence of decimal digits. Print the first digit,
// followed by a decimal
// dot '.' followed by the remaining digits. The caller has to guarantee that:
//   10^(olength-1) <= digits < 10^olength
// e.g., by passing `olength` as `decimalLength9(digits)`.
#if 0 // this will be used by d2exp_buffered_n()
static inline void
append_d_digits(const uint32_t olength, uint32_t digits, char* const result)
{
    uint32_t i = 0;
    while (digits >= 10000)
    {
        #ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
            const uint32_t c = digits - 10000 * (digits / 10000);
        #else
            const uint32_t c = digits % 10000;
        #endif
        digits /= 10000;
        const uint32_t c0 = (c % 100) << 1;
        const uint32_t c1 = (c / 100) << 1;
        memcpy(result + olength + 1 - i - 2, DIGIT_TABLE + c0, 2);
        memcpy(result + olength + 1 - i - 4, DIGIT_TABLE + c1, 2);
        i += 4;
    }
    if (digits >= 100)
    {
        const uint32_t c = (digits % 100) << 1;
        digits /= 100;
        memcpy(result + olength + 1 - i - 2, DIGIT_TABLE + c, 2);
        i += 2;
    }
    if (digits >= 10)
    {
        const uint32_t c = digits << 1;
        result[2] = DIGIT_TABLE[c + 1];
        result[1] = '.';
        result[0] = DIGIT_TABLE[c];
    }
    else
    {
        result[1] = '.';
        result[0] = (char) ('0' + digits);
    }
}
#endif

// Convert `digits` to decimal and write the last `count` decimal digits to result.
// If `digits` contains additional digits, then those are silently ignored.
static inline void
append_c_digits(const uint32_t count, uint32_t digits, char* const result)
{
    // Copy pairs of digits from DIGIT_TABLE.
    uint32_t i = 0;
    for (; i < count - 1; i += 2)
    {
        const uint32_t c = (digits % 100) << 1;
        digits /= 100;
        memcpy(result + count - i - 2, DIGIT_TABLE + c, 2);
    }
    // Generate the last digit if count is odd.
    if (i < count)
    {
        const char c = (char) ('0' + (digits % 10));
        result[count - i - 1] = c;
    }
}

// Convert `digits` to decimal and write the last 9 decimal digits to result.
// If `digits` contains additional digits, then those are silently ignored.
static inline void
append_nine_digits(uint32_t digits, char* const result)
{
    if (digits == 0)
    {
        memset(result, '0', 9);
        return;
    }

    for (uint32_t i = 0; i < 5; i += 4)
    {
        #ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
            const uint32_t c = digits - 10000 * (digits / 10000);
        #else
            const uint32_t c = digits % 10000;
        #endif
        digits /= 10000;
        const uint32_t c0 = (c % 100) << 1;
        const uint32_t c1 = (c / 100) << 1;
        memcpy(result + 7 - i, DIGIT_TABLE + c0, 2);
        memcpy(result + 5 - i, DIGIT_TABLE + c1, 2);
    }
    result[0] = (char) ('0' + digits);
}

static inline uint32_t indexForExponent(const uint32_t e)
{
    return (e + 15) / 16;
}

static inline uint32_t pow10BitsForIndex(const uint32_t idx)
{
    return 16 * idx + POW10_ADDITIONAL_BITS;
}

static inline uint32_t lengthForIndex(const uint32_t idx)
{
    // +1 for ceil, +16 for mantissa, +8 to round up when dividing by 9
    return (log10Pow2(16 * (int32_t) idx) + 1 + 16 + 8) / 9;
}

// ---------------------------------------------------------------------------
//
// START OF MODIFIED RYU

#include "pfstring.h"

static inline int
pf_copy_special_str_printf(
    struct PFString out[const static 1],
    const bool sign,
    const uint64_t mantissa,
    const bool uppercase)
{
    if (mantissa != 0)
    {
        concat(out, uppercase ? "NAN" : "nan", strlen("nan"));
        return out->length;
    }
    if (sign)
    {
        push_char(out, '-');
    }
    concat(out, uppercase ? "INF" : "inf", strlen("inf"));
    if (capacity_left(*out))
        out->data[out->length] = '\0';

    return out->length;
}

static unsigned
pf_d2fixed_buffered_n(
    char* const result,
    const size_t n,
    const PFFormatSpecifier fmt,
    const double d)
{
    struct PFString out = { result, .capacity = n };
    unsigned precision;
    if (fmt.precision.option == PF_SOME)
        precision = fmt.precision.width;
    else
        precision = 6;

    const uint64_t bits = double_to_bits(d);

    // Decode bits into sign, mantissa, and exponent.
    const bool ieeeSign =
        ((bits >> (DOUBLE_MANTISSA_BITS + DOUBLE_EXPONENT_BITS)) & 1) != 0;
    const uint64_t ieeeMantissa = bits & ((1ull << DOUBLE_MANTISSA_BITS) - 1);
    const uint32_t ieeeExponent = (uint32_t)
        ((bits >> DOUBLE_MANTISSA_BITS) & ((1u << DOUBLE_EXPONENT_BITS) - 1));

    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << DOUBLE_EXPONENT_BITS) - 1u))
    {
        const bool uppercase =
            fmt.conversion_format == 'F' || fmt.conversion_format == 'G';
        return pf_copy_special_str_printf(&out, ieeeSign, ieeeMantissa, uppercase);
    }

    if (ieeeExponent == 0 && ieeeMantissa == 0) // d == 0.0
    {
        if (ieeeSign)
            push_char(&out, '-');
        push_char(&out, '0');

        if (precision > 0 || fmt.flag.hash)
            push_char(&out, '.');
        pad(&out, '0', precision);

        if (capacity_left(out))
            out.data[out.length] = '\0';
        return out.length;
    }

    int32_t e2;
    uint64_t m2;
    if (ieeeExponent == 0)
    {
        e2 = 1 - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS;
        m2 = ieeeMantissa;
    }
    else
    {
        e2 = (int32_t) ieeeExponent - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS;
        m2 = (1ull << DOUBLE_MANTISSA_BITS) | ieeeMantissa;
    }

    bool is_zero = true; // for now
    if (ieeeSign)
        push_char(&out, '-');

    uint32_t all_digits[256] = {}; // significant digits without trailing zeroes
    size_t digits_length = 0;
    size_t integer_part_end = 0; // place for decimal point

    if (e2 >= -52) // store integer part
    {
        const uint32_t idx = e2 < 0 ? 0 : indexForExponent((uint32_t) e2);
        const uint32_t p10bits = pow10BitsForIndex(idx);
        const int32_t len = (int32_t)lengthForIndex(idx);

        for (int32_t i = len - 1; i >= 0; --i)
        {
            const uint32_t j = p10bits - e2;
            const uint32_t digits = mulShift_mod1e9(
                m2 << 8, POW10_SPLIT[POW10_OFFSET[idx] + i], (int32_t) (j + 8));

            if ( ! is_zero)
            { // always subsequent iterations of loop
                all_digits[digits_length++] = digits;
            }
            else if (digits != 0)
            { // always 1st iteration of loop
                all_digits[digits_length++] = digits;
                is_zero = false;
            }
        }
        integer_part_end = digits_length;
    }

    if (is_zero)
    {
        all_digits[0]    = 0;
        digits_length    = 1;
        integer_part_end = 1;
    }

    bool round_up = false;
    uint32_t lastDigit = 0; // to be cut off. Determines roundUp.
    uint32_t last_digit_magnitude = 1000*1000*1000;
    uint32_t maximum = 0;
    unsigned fract_leading_zeroes = 0;
    unsigned fract_trailing_zeroes = 0;

    if (e2 < 0) // store fractional part
    {
        const int32_t idx = -e2 / 16;
        const uint32_t blocks = precision / 9 + 1;

        uint32_t i = 0;
        if (blocks <= MIN_BLOCK_2[idx])
        {
            i = blocks; // skip the for-loop below
            fract_leading_zeroes = precision;
        }
        else if (i < MIN_BLOCK_2[idx])
        {
            i = MIN_BLOCK_2[idx];
            fract_leading_zeroes = 9 * i;
        }

        uint32_t digits;
        for (; i < blocks; ++i) // store significant fractional digits
        {
            const int32_t j = ADDITIONAL_BITS_2 + (-e2 - 16 * idx);
            const uint32_t p = POW10_OFFSET_2[idx] + i - MIN_BLOCK_2[idx];

            if (p >= POW10_OFFSET_2[idx + 1])
            {
                fract_trailing_zeroes = precision - 9 * i;
                break;
            }

            digits = mulShift_mod1e9(m2 << 8, POW10_SPLIT_2[p], j + 8);
            all_digits[digits_length++] = digits;
        }

        if (i == blocks)
        {
            maximum = precision - 9 * (i - 1);

            uint32_t k;
            for (k = 0; k < 9 - maximum; ++k) // trim digits from right
            {
                lastDigit = digits % 10;
                last_digit_magnitude /= 10;
                digits /= 10;
            }

            if (lastDigit != 5)
            {
                round_up = lastDigit > 5;
            }
            else
            {
                const bool any_left_in_digits = k < 9;
                const uint32_t next_digit = any_left_in_digits ?
                    digits : all_digits[digits_length - 2];

                const int32_t requiredTwos = -e2 - (int32_t) precision - 1;
                const bool trailingZeros = requiredTwos <= 0 || (
                    requiredTwos < 60 &&
                    multipleOfPowerOf2(m2, (uint32_t)requiredTwos)
                );

                round_up = next_digit % 2 || ! trailingZeros;
            }

            all_digits[digits_length - 1] = digits;
        }
    }

    if (round_up)
    {
        all_digits[digits_length - 1] += 1;

        if (all_digits[digits_length - 1] == last_digit_magnitude)
            all_digits[digits_length - 1] = 0; // carry 1
        else
            round_up = false;

        for (size_t i = digits_length - 2; i > 0; i--) // keep rounding
        {
            all_digits[i] += 1;
            if (all_digits[i] == (uint32_t)1000*1000*1000) {
                all_digits[i] = 0; // carry 1
            } else {
                round_up = false;
                break;
            }
        }

        if (round_up)
            all_digits[0] += 1;
    }

    // Start writing digits for integer part

    if (capacity_left(out) >= 9) // write directly
    {
        out.length += pf_utoa(
            capacity_left(out), out.data + out.length, all_digits[0]);
    }
    else // write only as much as fits
    {
        char buf[10];
        unsigned buf_len = pf_utoa(sizeof(buf), buf, all_digits[0]);
        concat(&out, buf, buf_len);
    }

    for (size_t i = 1; i < integer_part_end; i++)
    {
        if (capacity_left(out) >= 9) // write directly
        {
            append_nine_digits(all_digits[i], out.data + out.length);
            out.length += 9;
        }
        else // write only as much as fits
        {
            char buf[10];
            append_nine_digits(all_digits[i], buf);
            concat(&out, buf, 9);
        }
    }

    // Start writing digits for fractional part

    if (digits_length != integer_part_end && precision > 0)
    {
        push_char(&out, '.');
        pad(&out, '0', fract_leading_zeroes);

        for (size_t k = integer_part_end; k < digits_length - 1; k++)
        {
            if (capacity_left(out) >= 9) // write directly
            {
                append_nine_digits(all_digits[k], out.data + out.length);
                out.length += 9;
            }
            else // write only as much as fits
            {
                char buf[10];
                append_nine_digits(all_digits[k], buf);
                concat(&out, buf, 9);
            }
        }

        if (maximum > 0) // write the last digits left
        {
            if (capacity_left(out) >= maximum) // write directly
            {
                append_c_digits(
                    maximum, all_digits[digits_length - 1], out.data + out.length);
                out.length += maximum;
            }
            else // write only as much as fits
            {
                char buf[10];
                append_c_digits(
                    maximum, all_digits[digits_length - 1], buf);
                concat(&out, buf, maximum);
            }
        }

        pad(&out, '0', fract_trailing_zeroes);
    }
    else
    {
        pad(&out, '0', precision);
    }

    if (capacity_left(out))
        out.data[out.length] = '\0';
    return out.length;
}


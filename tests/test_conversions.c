#include "../src/conversions.c"
#include <gpc/assert.h>
#include "expect_str.h"

int main(void)
{
    gp_suite("Integer conversions");
    {
        char buf[MAX_DIGITS] = "";
        unsigned len = 0;

        gp_test("utoa");
        {
            len = pf_utoa(-1, buf, 1);
            expect_str(buf, "1");
            gp_expect(len == 1, (len));

            len = pf_utoa(-1, buf, 12345);
            expect_str(buf, "12345");
            gp_expect(len == 5, (len));
        }

        gp_test("itoa");
        {
            len = pf_itoa(-1, buf, 123456);
            expect_str(buf, "123456");
            gp_expect(len == 6, (len));

            len = pf_itoa(-1, buf, -123456);
            expect_str(buf, "-123456");
            gp_expect(len == 7, (len));
        }

        char buf2[MAX_DIGITS] = "";
        gp_test("otoa");
        {
            sprintf(buf2, "%o", 745);
            len = pf_otoa(-1, buf, 745);
            expect_str(buf, buf2);
            gp_expect(len == strlen(buf2));
        }

        gp_test("xtoa");
        {
            sprintf(buf2, "%x", 745);
            len = pf_xtoa(-1, buf, 745);
            expect_str(buf, buf2);
            gp_expect(len == strlen(buf2));
        }

        gp_test("Limit max characters");
        {
            strcpy(buf, "XXXXXX");
            pf_utoa(3, buf, 123456);
            expect_str(buf, "123XXX");

            strcpy(buf, "XXXXXX");
            pf_itoa(3, buf, -123456);
            expect_str(buf, "-12XXX");
        }
    } // gp_suite("Integer conversions");

    char buf[2000] = "";
    gp_suite("Float conversions");
    {
        double f = 0.;
        int return_value = 0;

        gp_test("ftoa basic tests");
        {
            f = 3.14;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            expect_str(buf, "3.140000");
            gp_expect(return_value == strlen("3.140000"), (return_value));

            f = 210123456789.0;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            expect_str(buf, "210123456789.000000");
            gp_expect(return_value == strlen("210123456789.000000"), (return_value));

            f = 0.123;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            expect_str(buf, "0.123000");
            gp_expect(return_value == strlen("0.123000"), (return_value));

            f = 0.012345678909876; // Note how 5 rounds up to 6
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            expect_str(buf, "0.012346");
            gp_expect(return_value == strlen("0.012346"), (return_value));

            f = -13999.99999999999;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            expect_str(buf, "-14000.000000");
            gp_expect(return_value == strlen("-14000.000000"), (return_value));

            f = -13999.99999999999;
            return_value = pf_ftoa(8, buf, f);
            buf[8] = '\0';
            expect_str(buf, "-14000.0");
            gp_expect(return_value == strlen("-14000.000000"), (return_value));
        }
    }

        // ----- INTERNALS ----- //

    gp_suite("pf_d2fixed"); //
    {
        double int64Bits2Double(uint64_t bits);
        double ieeeParts2Double(
            const bool sign,
            const uint32_t ieeeExponent,
            const uint64_t ieeeMantissa);
        #define EXPECT_FIXED(f, prec, fstr) do \
        { \
            const char* _fstr = (fstr); \
            PFFormatSpecifier fmt = \
                {.precision = {.option = PF_SOME, .width = (prec)} }; \
            unsigned return_value = \
                pf_d2fixed_buffered_n(buf, SIZE_MAX, fmt, (f)); \
            expect_str(buf, _fstr); \
            gp_expect(return_value == strlen(_fstr)); \
        } while (0);

        gp_test("Basic");
        {
            EXPECT_FIXED(
                ieeeParts2Double(false, 1234, 99999), 0,
                "3291009114715486435425664845573426149758869524108446525879746560");
        }
    }
}

double int64Bits2Double(uint64_t bits)
{
    double f;
    memcpy(&f, &bits, sizeof(double));
    return f;
}

double ieeeParts2Double(
    const bool sign,
    const uint32_t ieeeExponent,
    const uint64_t ieeeMantissa)
{
    assert(ieeeExponent <= 2047);
    assert(ieeeMantissa <= ((uint64_t)1 << 53) - 1);
    return int64Bits2Double(
        ((uint64_t)sign << 63) | ((uint64_t)ieeeExponent << 52) | ieeeMantissa);
}

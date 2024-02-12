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
        #define EXPECT_FIXED(f, prec, _expected) do \
        { \
            const char* expected = (_expected); \
            PFFormatSpecifier fmt = \
                {.precision = {.option = PF_SOME, .width = (prec)} }; \
            unsigned return_value = \
                pf_d2fixed_buffered_n(buf, SIZE_MAX, fmt, (f)); \
            expect_str(buf, expected); \
            gp_expect(return_value == strlen(expected)); \
        } while (0);

        gp_test("Basic");
        {
            EXPECT_FIXED(
                ieeeParts2Double(false, 1234, 99999), 0,
                "3291009114715486435425664845573426149758869524108446525879746560");
        }

        gp_test("Zero");
        {
            EXPECT_FIXED(0.0, 4, "0.0000");
            EXPECT_FIXED(0.0, 3, "0.000" );
            EXPECT_FIXED(0.0, 2, "0.00"  );
            EXPECT_FIXED(0.0, 1, "0.0"   );
            EXPECT_FIXED(0.0, 0, "0"     );
        }

        gp_test("MinMax");
        {
            EXPECT_FIXED(ieeeParts2Double(false, 0, 1), 1074,
                "0.000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000"
                "00000000000000049406564584124654417656879286822137236505980261"
                "43247644255856825006755072702087518652998363616359923797965646"
                "95445717730926656710355939796398774796010781878126300713190311"
                "40452784581716784898210368871863605699873072305000638740915356"
                "49843873124733972731696151400317153853980741262385655911710266"
                "58556686768187039560310624931945271591492455329305456544401127"
                "48012970999954193198940908041656332452475714786901472678015935"
                "52386115501348035264934720193790268107107491703332226844753335"
                "72083243193609238289345836806010601150616980975307834227731832"
                "92479049825247307763759272478746560847782037344696995336470179"
                "72677717585125660551199131504891101451037862738167250955837389"
                "73359899366480994116420570263709027924276754456522908753868250"
                "6419718265533447265625");

            EXPECT_FIXED(ieeeParts2Double(false, 2046, 0xFFFFFFFFFFFFFu), 0,
                "17976931348623157081452742373170435679807056752584499659891747"
                "68031572607800285387605895586327668781715404589535143824642343"
                "21326889464182768467546703537516986049910576551282076245490090"
                "38932894407586850845513394230458323690322294816580855933212334"
                "8274797826204144723168738177180919299881250404026184124858368");
        }

        gp_test("No rounding past leading fractional zeroes");
        {
            EXPECT_FIXED(0.00000000099999999, 12, "0.000000001000");
        }

        gp_test("Write many zeroes");
        {
            EXPECT_FIXED(0., 20, "0.""00000""00000""00000""00000");
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

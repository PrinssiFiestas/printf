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

    gp_suite("Float conversions");
    {
        char buf[2000] = "";

        double f = 0.;
        int return_value = 0;

        gp_test("ftoa normals");
        {
            f = 3.14;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            gp_expect(memcmp(buf, "3.140000", strlen("3.140000")) == 0, (buf));
            gp_expect(return_value == strlen("3.140000"), (return_value));

            f = 210123456789.0;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            gp_expect(memcmp(buf, "210123456789.000000", strlen("210123456789.000000")) == 0, (buf));
            gp_expect(return_value == strlen("210123456789.000000"), (return_value));

            f = 0.123;
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            gp_expect(memcmp(buf, "0.123000", strlen("0.123000")) == 0, (buf));
            gp_expect(return_value == strlen("0.123000"), (return_value));

            f = 0.012345678909876; // Note how 5 rounds up to 6
            return_value = pf_ftoa(SIZE_MAX, buf, f);
            gp_expect(memcmp(buf, "0.012346", strlen("0.012346")) == 0, (buf));
            gp_expect(return_value == strlen("0.012346"), (return_value));

            #if TODO
            f = -13999.99999999999;
            return_value = pf_ftoa(8, buf, f);
            gp_expect(memcmp(buf, "0.012346", strlen("0.012346")) == 0, (buf));
            gp_expect(return_value == strlen("0.012346"), (return_value));
            #endif

            // Test rounding from truncated string
            #if TODO
            f = 0.123456;
            return_value = pf_ftoa(5, buf, f);
            gp_expect(memcmp(buf, "0.12346", strlen("0.12346")) == 0, (buf));
            gp_expect(return_value == strlen("0.12346"), (return_value));
            #endif
        }
    }
}

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
    } // gp_suite("Integer conversions");

    gp_suite("Float conversions");
    {
        char buf[2000] = "";
        //char buf_std[sizeof(buf)] = "";

        double f = 0.;
        int return_value = 0;

        gp_test("ftoa normals");
        {
            f = 3.14;
            return_value = pf_ftoa(SIZE_MAX, buf, 6, f);
            gp_expect(0,
                (buf), (return_value));
        }
    }
}

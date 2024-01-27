#include <gpc/assert.h>
#include "../src/printf.c"
#include "expect_str.h"

int main(void)
{
    char buf[256] = "";
    char buf_std[256] = "";

    gp_suite("Basic type conversions");
    {
        gp_test("%c");
        {
            pf_sprintf(buf, "blah %c blah", 'x');
            expect_str(buf, "blah x blah");
        }

        gp_test("%s");
        {
            pf_sprintf(buf, "blah %s blah", "bloink");
            expect_str(buf, "blah bloink blah");
        }

        gp_test("%d and %i");
        {
            pf_sprintf(buf, "blah %d blah", 15);
            expect_str(buf, "blah 15 blah");

            pf_sprintf(buf, "blah %i blah", -953);
            expect_str(buf, "blah -953 blah");

            pf_sprintf(buf,  "blah %lli blah", -LLONG_MAX + 5);
            sprintf(buf_std, "blah %lli blah", -LLONG_MAX + 5);
            expect_str(buf, buf_std);
        }

        gp_test("%o, %x, and %X");
        {
            pf_sprintf(buf,  "blah %o blah", 384);
            sprintf(buf_std, "blah %o blah", 384);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %lx blah", 0xfeedl);
            sprintf(buf_std, "blah %lx blah", 0xfeedl);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %X blah", 0xBEEF);
            sprintf(buf_std, "blah %X blah", 0xBEEF);
            expect_str(buf, buf_std);
        }

        gp_test("floats");
        {
            pf_sprintf(buf,  "blah %f blah", 124.647);
            sprintf(buf_std, "blah %f blah", 124.647);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %E blah", -42e6);
            sprintf(buf_std, "blah %E blah", -42e6);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %g blah", -13.1);
            sprintf(buf_std, "blah %g blah", -13.1);
            expect_str(buf, buf_std);

        }
    } // gp_suite("Basic type conversions");

    //gp_suite("Precision");
    {
    }
}

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
        }

        gp_test("%o, %x, and %X");
        {
            pf_sprintf(buf,  "blah %o blah", 384);
            sprintf(buf_std, "blah %o blah", 384);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %x blah", 0xbeef);
            sprintf(buf_std, "blah %x blah", 0xbeef);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "blah %X blah", 0xFEED);
            sprintf(buf_std, "blah %X blah", 0xFEED);
            expect_str(buf, buf_std);
        }
    }
}

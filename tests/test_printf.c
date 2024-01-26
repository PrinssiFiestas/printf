#include <gpc/assert.h>
#include "../src/printf.c"
#include "expect_str.h"

int main(void)
{
    char buf[256] = "";
    //char buf_std[256] = "";

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
    }
}

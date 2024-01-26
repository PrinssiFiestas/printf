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
    }
}

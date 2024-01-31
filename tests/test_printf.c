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

            pf_sprintf(buf, "blah %ti blah", -953);
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

            pf_sprintf(buf,  "blah %zX blah", 0xBEEF);
            sprintf(buf_std, "blah %zX blah", 0xBEEF);
            expect_str(buf, buf_std);
        }

        gp_test("Floats");
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

        gp_test("%p");
        {
            void* p = (void*)-1;
            uintptr_t u = (uintptr_t)p;
            char _buf[MAX_DIGITS];
            const char* fmt =
                UINTPTR_MAX == ULLONG_MAX ? "%#llx" :
                UINTPTR_MAX == ULONG_MAX  ? "%#lx"  : "%#x";
            sprintf(_buf, fmt, u);
            pf_sprintf(buf, "%p", p);
            expect_str(buf, _buf);
        }
    } // gp_suite("Basic type conversions");

    gp_suite("Precision");
    {
        gp_test("Unsigned integers");
        {
            pf_sprintf(buf,  "%.4u", 3);
            sprintf(buf_std, "%.4u", 3);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%.24x", 0xe);
            sprintf(buf_std, "%.24x", 0xe);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%.*X", 3, 0XD);
            sprintf(buf_std, "%.*X", 3, 0XD);
            expect_str(buf, buf_std);
        }

        gp_test("Signed integers");
        {
            pf_sprintf(buf,  "%.3i", 2);
            sprintf(buf_std, "%.3i", 2);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%.3i", -2);
            sprintf(buf_std, "%.3i", -2);
            expect_str(buf, buf_std);
        }

        gp_test("Strings");
        {
            pf_sprintf(buf, "%.5s", "str");
            expect_str(buf, "str");

            pf_sprintf(buf, "%.5s", "String loger than 5 chars");
            expect_str(buf, "Strin");

            pf_sprintf(buf, "%.*s", 4, "String loger than 5 chars");
            expect_str(buf, "Stri");
        }
    } // gp_suite("Precision");

    gp_suite("Flags");
    {
        gp_test("#");
        {
            pf_sprintf(buf,  "%#x", 0);
            sprintf(buf_std, "%#x", 0);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%#.3X", 0xa);
            sprintf(buf_std, "%#.3X", 0xa);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%#.f", 1.0);
            sprintf(buf_std, "%#.f", 1.0);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%#g", 700.1);
            sprintf(buf_std, "%#g", 700.1);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%#g", 123456.0);
            sprintf(buf_std, "%#g", 123456.0);
            expect_str(buf, buf_std);
        }
    } // gp_suite("Flags");

    gp_suite("Misc");
    {
        gp_test("Return value");
        {
            unsigned chars_written = 0;
            unsigned chars_written_std = 0;

            chars_written     = pf_sprintf(buf,  "%s blah", "bloink");
            chars_written_std = sprintf(buf_std, "%s blah", "bloink");
            gp_expect(chars_written == chars_written_std,
                (chars_written), (chars_written_std));

            chars_written     = pf_sprintf(buf,  "blah %g", -2./9.);
            chars_written_std = sprintf(buf_std, "blah %g", -2./9.);
            gp_expect(chars_written == chars_written_std,
                (chars_written), (chars_written_std));
        }
    } // gp_suite("Misc");
}

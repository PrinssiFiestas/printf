#include "../src/conversions.c"
#include <gpc/assert.h>

// Must be a macro to get correct __LINE__ and __FILE__ for gp_expect()
#define expect_str(str_a, str_b) \
({ \
    const char *_str_a = (str_a), *_str_b = (str_b); \
    bool is_true = strcmp(_str_a, _str_b) == 0; \
    gp_expect(!!#str_a " equals " #str_b == is_true, (_str_a), (_str_b)); \
})

int main(void)
{
    gp_suite("Integer conversions");
    {
        char buf[MAX_DIGITS] = "";

        gp_test("utoa");
        {
            expect_str(pf_utoa(buf, 1), "1");
            expect_str(pf_utoa(buf, 12345), "12345");
        }

        gp_test("itoa");
        {
            expect_str(pf_itoa(buf, 123456), "123456");
            expect_str(pf_itoa(buf, -123456), "-123456");
        }

        char buf2[MAX_DIGITS] = "";
        gp_test("otoa");
        {
            sprintf(buf2, "%o", 745);
            expect_str(pf_otoa(buf, 745), buf2);
        }

        gp_test("xtoa");
        {
            sprintf(buf2, "%x", 745);
            expect_str(pf_xtoa(buf, 745), buf2);
        }
    } // gp_suite("Integer conversions");

    // TODO
    // gp_suite("Float conversions");
    {

        // ---------- Internal tests ----------- //

        // Change the values for the #if directives to test float endianness.
        // Note: Endiannes of floats might differ from integer endiannes!
        #if 0
        gp_test("Float endiannes");
        {
            #if 1 // little endian
                gp_expect(little_endian_double());
            #else // big endian
                gp_expect( ! little_endian_double());
            #endif
        }
        #endif
    }
}

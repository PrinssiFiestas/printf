#include "../src/conversions.c"
#include <gpc/assert.h>

#if defined(__GNUC__) && defined(__linux__)
#include <endian.h>
#endif

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

    // --------------------------------------------------------------------
    // Testing internals

    gp_suite("Endiannes test");
    {
        gp_test("is_little_endian");
        {
        #if defined(__GNUC__) && defined(__linux__)
            #if __BYTE_ORDER == __LITTLE_ENDIAN
                gp_expect(is_little_endian());
            #else
                gp_expect( ! is_little_endian());
            #endif
        #else
            // Uncomment whichever is true in your system
            //gp_expect(is_little_endian());
            //gp_expect( ! is_little_endian());
        #endif
        }
    }
}

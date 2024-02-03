#include <gpc/assert.h>
#include "../src/printf.c"
#include "expect_str.h"
#include "pcg_basic.h"
#include <time.h>

int main(void)
{
    char buf[512] = "";
    char buf_std[512] = "";

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
        gp_test("-: Left justification");
        {
            pf_sprintf(buf, "|%-8i|", -2);
            expect_str(buf, "|-2      |");
        }

        gp_test("+: Add plus to signed positives");
        {
            pf_sprintf(buf, "%+i", 35);
            expect_str(buf, "+35");

            pf_sprintf(buf, "%+g", 1.41);
            expect_str(buf, "+1.41");
        }

        gp_test(" : Add space to signed positives");
        {
            pf_sprintf(buf, "% i", 35);
            expect_str(buf, " 35");

            pf_sprintf(buf, "% g", 1.41);
            expect_str(buf, " 1.41");
        }

        gp_test("0: Zero padding");
        {
            pf_sprintf(buf, "|%08i|", -1);
            expect_str(buf, "|-0000001|");
        }

        gp_test("#: Alternative form");
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

    gp_suite("Fields");
    {
        gp_test("Basic field");
        {
            pf_sprintf(buf, "|%#8x|", 0x3);
            expect_str(buf, "|     0x3|");
        }
    }

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

        gp_test("Combinations");
        {
            pf_sprintf(buf,  "blah %f, %#0x", .5, 0x2);
            sprintf(buf_std, "blah %f, %#0x", .5, 0x2);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "%.3s, %+4i", "bloink", 63);
            sprintf(buf_std, "%.3s, %+4i", "bloink", 63);
            expect_str(buf, buf_std);

            pf_sprintf(buf,  "% 04i", 21);
            sprintf(buf_std, "% 04i", 21);
            expect_str(buf, buf_std);
        }

        gp_test("No format specifier");
        {
            pf_sprintf(buf, "Whatever");
            expect_str(buf, "Whatever");
        }

        gp_test("%%");
        {
            pf_sprintf(buf, "%% blah");
            expect_str(buf, "% blah");

            pf_sprintf(buf, "blah %%");
            expect_str(buf, "blah %");

            pf_sprintf(buf, "bl%%ah");
            expect_str(buf, "bl%ah");
        }
    } // gp_suite("Misc");

    gp_suite("Fuzz test");
    {
        // Seed RNG with date
        {
            time_t t = time(NULL);
            struct tm* gmt = gmtime(&t);
            gp_assert(gmt != NULL);
            pcg32_srandom(gmt->tm_mday + 100*gmt->tm_mon, gmt->tm_year);
        }
        const unsigned loop_count = 2048 * 8;
        const char* random_format(char conversion_type);

        gp_test("Random formats with random values");
        {
            for (unsigned iteration = 1; iteration <= loop_count; iteration++)
            {
                uintmax_t random_bytes; // bit width assumed to be multiple of 32
                for (uint32_t* i = (uint32_t*)&random_bytes;
                    (uintmax_t*)i < &random_bytes + 1;
                    i++) {
                    *i = pcg32_random();
                }
                const char* all_specs = "diouxXeEfFgGaAcsp"; // exept 'n'
                const char random_specifier =
                    all_specs[pcg32_boundedrand(strlen(all_specs))];
                const char* fmt = random_format(random_specifier);

                int _my_buf_return_value = 0;
                int buf_std_return_value = 0;

                if (random_specifier != 's')
                {
                    _my_buf_return_value = pf_sprintf(buf,  fmt, random_bytes);
                    buf_std_return_value = sprintf(buf_std, fmt, random_bytes);
                }
                else // treat random_bytes as string
                {
                    ((char*)&random_bytes)[sizeof(uintmax_t) - 1] = '\0';
                    _my_buf_return_value = pf_sprintf(buf,  fmt, &random_bytes);
                    buf_std_return_value = sprintf(buf_std, fmt, &random_bytes);
                }
                // Rename buf for aligned gp_assert() message
                const char* _my_buf = buf;
                gp_assert(strcmp(buf, buf_std) == 0,
                    (fmt),
                    ("%#jx", random_bytes),
                    (_my_buf),
                    (buf_std),
                    (iteration));

                gp_assert(_my_buf_return_value == buf_std_return_value,
                    (_my_buf_return_value), (buf_std_return_value));
            }
        }
    }
}

bool coin_flip()
{
    return pcg32_boundedrand(2);
}

const char* random_format(char conversion_type)
{
    static size_t fmt_capacity = 128;
    static char* fmt;
    static bool initialized = false;

    if ( ! initialized)
    {
        fmt = malloc(fmt_capacity);
        initialized = true;
    }
    memset(fmt, 0, fmt_capacity);
    size_t fmt_i = 0;

    #define push_char(c) do \
    { \
        fmt[fmt_i] = (c); \
        fmt_i++; \
        if (fmt_i >= fmt_capacity - sizeof("x")) \
        { \
            char* new_buf = calloc(2, fmt_capacity); \
            gp_assert(new_buf != NULL); \
            memcpy(new_buf, fmt, fmt_capacity); \
            free(fmt); \
            fmt = new_buf; \
            fmt_capacity *= 2; \
        } \
    } while (0);

    push_char('%');

    char flags[8] = "-"; // dash is common for all
    switch (conversion_type)
    {
        // signed conversions
        case 'd': case 'i': case 'a': case 'A':
        case 'e': case 'E': case 'f': case 'F':
        case 'g': case 'G':
            strcat(flags, "0 +");
            if ( ! (conversion_type == 'd' || conversion_type == 'i'))
                strcat(flags, "#");
            break;

        // unsinged conversions
        case 'o': case 'u': case 'x': case 'X':
            strcat(flags, "#0");
            break;

        case 'c': case 's': case 'p':
            break;

        default:
            strcpy(fmt, "Invalid conversion format character!");
            return fmt;
    }

    while (coin_flip()) // add random flags
    {
        push_char(flags[pcg32_boundedrand(strlen(flags))]);
    }

    if (coin_flip()) // add random field width
    {
        push_char(pcg32_boundedrand(9) + '1');
        if (coin_flip())
            push_char(pcg32_boundedrand(10) + '0');
            // no need to go past 100
    }

    if (coin_flip() && conversion_type != 'c') // add random precision
    {
        push_char('.');
        push_char(pcg32_boundedrand(9) + '1');
        if (coin_flip())
            push_char(pcg32_boundedrand(10) + '0');
    }

    fmt[fmt_i] = conversion_type;
    fmt[fmt_i + 1] = '\0';
    return fmt;
}

#include "pcg_basic.c"

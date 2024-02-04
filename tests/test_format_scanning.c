#include "../src/format_scanning.c"
#include <gpc/assert.h>

int main(void)
{
    gp_suite("Format parsing");
    {
        PFFormatSpecifier fmt =
            scan_format_string("blah blah %#035.721hhg blah", NULL);

        gp_test("Format string");
        {
            const char* _fmt = "%#035.721hhg";
            gp_expect(strncmp(fmt.string, _fmt, fmt.string_length) == 0,
                ("%.12s", fmt.string));

            gp_expect(fmt.string_length == strlen(_fmt));
        }

        gp_test("Flags");
        {
            gp_expect(fmt.flag.hash && fmt.flag.zero);
            gp_expect( ! (fmt.flag.plus || fmt.flag.space || fmt.flag.dash));
        }

        gp_test("Field width");
        {
            gp_expect(fmt.field.width == 35, (fmt.field.width));
            gp_expect( ! fmt.field.asterisk);
        }

        gp_test("Precision");
        {
            gp_expect(fmt.precision.width == 721, (fmt.precision.width));
            gp_expect(fmt.precision.option == PF_SOME);
        }

        gp_test("Type");
        {
            gp_expect(fmt.length_modifier == 2 * 'h', (fmt.length_modifier));
            gp_expect(fmt.conversion_format == 'g');
        }
    }
}

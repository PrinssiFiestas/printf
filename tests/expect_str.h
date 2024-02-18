#ifndef EXPECT_STR_H_INCLUDED
#define EXPECT_STR_H_INCLUDED

#define expect_str(str_a, str_b) \
({ \
    const char *_str_a = (str_a), *_str_b = (str_b); \
    bool is_true = strcmp(_str_a, _str_b) == 0; \
    gp_expect(!!#str_a " equals " #str_b == is_true, (_str_a), (_str_b)); \
})

#define assert_str(str_a, str_b) \
({ \
    const char *_str_a = (str_a), *_str_b = (str_b); \
    bool is_true = strcmp(_str_a, _str_b) == 0; \
    gp_assert(!!#str_a " equals " #str_b == is_true, (_str_a), (_str_b)); \
})

#endif // EXPECT_STR_H_INCLUDED

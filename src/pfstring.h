#ifndef PFSTRING_H_INCLUDED
#define PFSTRING_H_INCLUDED

#include <printf/printf.h>

struct PFString
{
    // length is used to store the return value of printf() so it may exceed
    // capacity.

    char* data;
    size_t length;
    const size_t capacity;
};

static inline size_t capacity_left(const struct PFString me)
{
    return me.length >= me.capacity ? 0 : me.capacity - me.length;
}

// Useful for memcpy(), memmove(), memset(), etc.
static inline size_t limit(const struct PFString me, const size_t x)
{
    const size_t cap_left = capacity_left(me);
    return cap_left < x ? cap_left : x;
}

// Mutating functions return successfully written characters.

static inline size_t concat(struct PFString* me, const char* src, const size_t length)
{
    memcpy(me->data + me->length, src, limit(*me, length));
    me->length += length;
    return limit(*me, length);
}

static inline size_t pad(struct PFString* me, const char c, const size_t length)
{
    memset(me->data + me->length, c, limit(*me, length));
    me->length += length;
    return limit(*me, length);
}

static inline bool push_char(struct PFString* me, const char c)
{
    if (limit(*me, 1) != 0)
        me->data[me->length] = c;
    me->length++;
    return limit(*me, 1);
}

#endif // PFSTRING_H_INCLUDED

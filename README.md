# Printf

Clone of `printf()`, `fprintf()`, `sprintf()`, `snprintf()`, and their 'v' variants. Implements close to all features required by ANSI C. 

All provided functions live in `pf_` namespace. Check the headers in `include/printf/`. 

## Requirements

Make and GNU C99 compatible compiler. C11 is required for tests. C++ not supported at the moment.

## Docs

Since `pf_printf()`is ANSI C compatible, just refer to [the standard](https://web.archive.org/web/20200909074736if_/https://www.pdf-archive.com/2014/10/02/ansi-iso-9899-1990-1/ansi-iso-9899-1990-1.pdf) page 131. Man pages is also fine, but just know that C99 `%a` and `%A` and non-standard extensions are not supported. 

## What's special

### Float conversion superiority

This is the first fully functional `printf()` using [the fast Ryū algorithm](https://github.com/ulfjack/ryu) for floating point conversions, as far as I'm aware. In addition to speed, there is no compromising correctness. Most other implementations just print zeroes or even wrong values when precision gets cranked. Thanks to Ryū, arbitrary precisions are supported without compromising speed. 

### No allocations

String functions `sprintf()`, `snprintf()`, `vsprintf()`, and `vsnprintf()`are guaranteed to not allocate and are reentrant. The other ones will allocate once if the output exceeds 4096 bytes which shouldn't be a big deal for IO operations. 

### Well tested

`pf_snprintf()`has a comprehensive set of unit tests comparing the output against glibc `snprintf()`. The included fuzz test mostly just finds bugs in glibc floating point conversions. 

### Open guts

This was the reason for writing this library in the first place. The library does not just provide `printf()`clones, but also the conversion functions as well as a format string scanner is provided. They should make it trivial to write your own custom lightweight logging functions. Again, check the headers. 

## Limitations

Poorly supported inconsistent `long double` and useless security hole `%n` are not supported. 

Standard `printf()`returns a negative number on errors, usually for invalid formats. `pf_printf()`does handle any errors at the moment. But lets face it; your lazy arse wouldn't check them anyway. You disgust me.

## Todo

Optimizing parts of the library is a work in progress. 

Some guts could be modularized even better for better flexibility for custom loggers. 

Better compiler support.

The overengineered Makefile is riddled with bugs. Not my fault, Make was written by aliens on space fumes. 

## Other notes

Thanks to Ulf Adams for creating Ryū algorithm! 

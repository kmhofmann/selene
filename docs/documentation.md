# Selene

## Documentation

**Selene** is fully documented using the [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) format.
_Doxygen_ documentation can be built using the provided `Doxyfile`.
Simply run `doxygen` in the main library directory.

A regularly updated version of the API documentation [is hosted here](https://selene-lib.org/docs/).

Commented example code for select functionality (which can serve as a tutorial) can be found in the
[`examples` folder of the repository](../examples). (To be extended)

## Status

**Selene** requires a compiler supporting C++14 and a respective conforming standard library implementation.

The library is mainly developed on 
- Ubuntu Linux using the latest GCC and Clang versions (currently: GCC 7.2.0 and Clang 6.0.0)
- MacOS using the latest AppleClang version (currently: 9.0.0).

Continuous integration tests are currently being run on:
- Ubuntu Linux, using GCC 5.4.1, GCC 6.4.0, and GCC 7.2.0 (via [Travis CI](https://travis-ci.org/)).
- Windows, using Visual Studio 2017 (via [Appveyor](https://www.appveyor.com/)).

Continuous integration tests using Clang (and with sanitizers enabled) are on the "would be great" list, but setting
these up with [Travis CI](https://travis-ci.org/)'s hopelessly outdated Ubuntu images is exceedingly difficult, so it might not happen for a
while.
(The main difficulty is in using a recent version of `libc++` or `libstdc++` with Clang.)

The library is work-in-progress and released as an early preview, and shall be considerably extended and modified in
the future; in particular by providing further image processing capabilities.

Therefore, no API or ABI stability guarantees are currently given.
However, the code is well-tested and assumed to be not too far from production-level quality.
Adoption and usage is explicitly encouraged.

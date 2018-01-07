# Selene

[Main page](https://github.com/kmhofmann/selene)

## Documentation

**Selene** is fully documented using the [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) format.
_Doxygen_ documentation can be built using the provided `Doxyfile`.
Simply run `doxygen` in the main library directory.

A regularly updated version of the API documentation [is hosted here](https://michael-hofmann.info/selene/).

Commented example code for select functionality (which can serve as a tutorial) can be found in the
[`examples` folder of the repository](../examples). (To be extended)

## Status

**Selene** requires a compiler supporting C++14 and a respective conforming standard library implementation.

It is currently developed and tested on
- Linux using GCC 5.4.0, GCC 7.2.0 and Clang 6.0.0 (trunk)
- MacOS using AppleClang 9.0.0.
- Windows using Visual C++ 2017 (compiler version >= 19.12.25831).

The library is work-in-progress and released as an early preview, and shall be considerably extended and modified in
the future; in particular by providing further image processing capabilities.

Therefore, no API or ABI stability guarantees are currently given.
However, the code is well-tested and assumed to be not too far from production-level quality.
Adoption and usage is explicitly encouraged.
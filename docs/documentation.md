# Selene

## Documentation

**Selene** is (almost) fully documented using the [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) format.
_Doxygen_ documentation can be built using the provided `Doxyfile`.
Simply run `doxygen` in the main library directory.

* A regularly updated version of the API documentation [is hosted here](https://selene-lib.org/docs/).

## Examples

Commented example code for select functionality (which can serve as a tutorial) can be found in the
[`examples` folder of the repository](../examples).

## Platform and compiler support

**Selene** requires a compiler supporting C++17 and a respective conforming standard library implementation.
As such, it can be built on the usual major plaforms: **Linux**, **MacOS**, **Windows**.

There are no obvious reasons why **Selene** could not be compiled on other platforms, such as **Android** or **iOS**.
However, these platforms are not within the testing scope.

Continuous integration tests are currently being run on:
- Ubuntu Linux 18.10  (via [Azure DevOps](https://dev.azure.com)), using the following compilers:
    - GCC 8.2
    - Clang 7.0 (with and without AddressSanitizer)
- Windows, using Visual Studio 2017 (via [Appveyor](https://www.appveyor.com/)).

## Work in progress

The library is work-in-progress and released as an early preview, and shall be considerably extended and modified in
the future; in particular by providing further image processing capabilities.

Therefore, no API or ABI stability guarantees are currently given.
Changes in API for versions less than `1.0` may occur at any time and break backwards compatibility.

However, the code is fairly well-tested and assumed to be not too far from production-level quality.
Adoption and usage are explicitly encouraged.

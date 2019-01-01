# Selene

## Documentation

**Selene** is (almost) fully documented using the [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) format.
_Doxygen_ documentation can be built using the provided `Doxyfile`.
Simply run `doxygen` in the main library directory.

* A regularly updated version of the API reference documentation [is hosted here](https://selene-lib.org/docs/).

## Example code

Commented example code for select functionality can be found in the [`examples` folder of the repository](../examples).

These examples can serve as a tutorial, and are probably the easiest starting point to get to know important parts of
the library. 

## Platform and compiler support

**Selene** requires a compiler supporting C++17 and a respective conforming standard library implementation.
As such, it can be built on the usual major plaforms: **Linux**, **macOS**, **Windows**.

There are no obvious reasons why **Selene** could not be compiled on other platforms, such as **Android**, **iOS**,
**FreeBSD**, etc.
However, these platforms are not within the testing scope.

Continuous integration tests are currently being run on the following platforms, via [Azure DevOps](https://dev.azure.com):
- Ubuntu Linux 18.10, using the following compilers:
    - GCC 8.2
    - Clang 7.0 (with and without AddressSanitizer and UndefinedBehaviorSanitizer)
- Windows 10
    - Visual Studio 2017.

## Work in progress

**Selene** is work-in-progress and released as an early preview, and shall be considerably extended and modified in
the future; in particular by providing further image processing capabilities and optimizations.

Therefore, no API or ABI stability guarantees are currently given.
API changes for versions less than `1.0` may occur at any time and break backwards compatibility.

However, the code is fairly well-tested and assumed to be not too far from production-level quality.
Adoption and usage are explicitly encouraged, bearing in mind that changes in code that uses **Selene** may be necessary.

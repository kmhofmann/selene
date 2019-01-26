# Selene

## Platform and compiler support

**Selene** requires a compiler supporting C++17 and a respective conforming standard library implementation.
As such, it can be built on the usual major platforms, as long as these requirements are satisfied.

The library is regularly tested on **Linux**, **macOS**, **Windows**, and, to a limited degree, **Android**
(just compilation with the Android NDK).
There are no obvious reasons why **Selene** could not be compiled on other platforms, such as **iOS**,
**FreeBSD**, etc.
However, these platforms are not within the testing scope (via continuous integration or otherwise).

## Continuous integration testing

Continuous integration tests are currently being run on the following platforms, via
[Azure DevOps](https://dev.azure.com):
- Ubuntu Linux 18.10, using the following compilers:
    - GCC 8.2
    - Clang 7.0 (both with and without AddressSanitizer &amp; UndefinedBehaviorSanitizer)
    - Android NDK r19, Clang, arm64 (just compilation; not running unit tests, in absence of ARM platform)
- Windows 10
    - Visual Studio 2017.

(CI testing on macOS is momentarily disabled, due to Azure DevOps not providing a macOS 10.14 image yet, for
up-to-date compiler and standard library support. Once this is provided, CI testing on macOS shall be
reinstated.)

## Work in progress

**Selene** is work-in-progress and released as a preview.
The library shall be extended and modified in the future; see below.
As such, several changes in architecture and API are likely to be made.

Therefore, **no API or ABI stability guarantees are currently given**.
API changes for versions less than `1.0` may occur at any time and break backwards compatibility.

That all said, adoption and usage are explicitly encouraged, bearing in mind that changes in code that
uses **Selene** may be necessary.

## Roadmap

At the moment, the project does not have a proper roadmap, as it currently just has one major contributor working on
it during his spare time.
(But who would love to see the library grow, mature, and be adopted. [Contributions](../CONTRIBUTING.md)
are welcome! :-)) 

A rough sketch of what might be planned for future releases &mdash; no guarantees given:
* Proper allocator support
* Fast image rescaling algorithms
* Lazy view evaluation
* Planar image data representation
* Improved generic image operation algorithms
* SIMD vectorization of operations (using helper library, e.g. Vc)

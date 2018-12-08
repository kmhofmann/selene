# Selene

## Library dependencies

**Selene** uses the following (optional) third-party dependencies for implementing some of its functionality:

  - [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) or [libjpeg](http://www.ijg.org/):
    - Optional, but recommended.
    - Required for the JPEG reading and writing API.
    - `libjpeg-turbo` is the preferred version of the library.
  - [libpng](http://www.libpng.org/pub/png/libpng.html):
    - Optional, but recommended.
    - Required for the PNG reading and writing API.
  - [OpenCV](https://opencv.org/):
    - Optional, if needed.
    - For OpenCV interoperability (e.g. copying or wrapping image data).
    - **Not required** for building the library, since OpenCV interoperability is implemented in a header file only.

The presence (or lack of) these dependencies should be detected automatically by CMake.

If [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) is present on the system (as opposed to IJG *libjpeg*),
some of its additional capabilities (such as partial JPEG image decoding and extended color spaces) will be taken into
account and are enabled by the API.

[OpenCV](https://opencv.org/) is only needed for converting between `sln::Image<T>` and OpenCV's `cv::Mat` structure, if
so desired.
It is by no means a requirement for using **Selene**.

To point CMake to custom library installation locations, set the `CMAKE_PREFIX_PATH` environment variable accordingly.
For example, `export CMAKE_PREFIX_PATH=$HOME/local/libjpeg-turbo:$HOME/local/libpng`. 

The use of [Conan](https://conan.io/) or [vcpkg](https://github.com/Microsoft/vcpkg) as dependency package managers is
also supported; see below.

## Auxiliary dependencies

* The test suite depends on [Catch2](https://github.com/catchorg/Catch2) and [Boost.Filesystem](http://www.boost.org/)
for building the tests.
  The former is bundled as a Git submodule and will be automatically cloned during execution of the `cmake` command.
  The latter is automatically searched for by a CMake `find_package` command; its presence is required to build the tests.

* The benchmark programs depend on the [Google benchmark](https://github.com/google/benchmark) library.
  It is searched for by a CMake `find_package` command.


## Installing dependencies

The following are recommendations for installation of dependencies on various platforms.
Of course any dependency can alternatively also be built and installed from source, as long as it can be found by the
CMake `find_package` command.

### All platforms (Linux, MacOS, Windows)

**Recommended**: Use a package manager such as [Conan](https://conan.io/) or
[vcpkg](https://github.com/Microsoft/vcpkg).

This is the preferred way to install dependencies, over system-level package managers such as `apt`.
The latter often supply outdated packages.

See below for instructions on how to install dependencies using [Conan](https://conan.io/) or
[vcpkg](https://github.com/Microsoft/vcpkg).

### Linux

On Debian-like systems (e.g. Ubuntu), you should be able to use `apt` as follows:

    # Dependencies for building the library
    apt install libjpeg-turbo8-dev libpng-dev
    
    # Dependencies for building tests & examples
    apt install libopencv-dev libboost-filesystem-dev
    
    # Note: There is no pre-built google-benchmark package in e.g. Ubuntu.
    #       Install from source instead, or use vcpkg.

### MacOS

Install [Homebrew](https://brew.sh/) to build and install the dependencies as follows:

    # Dependencies for building the library
    brew install libjpeg-turbo libpng
    
    # Dependencies for building tests, examples, benchmarks
    brew install opencv3 boost google-benchmark

### Windows

Use [vcpkg](https://github.com/Microsoft/vcpkg); see below.

Set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` before installing the above packages to install
the 64-bit compiled versions instead of the 32-bit ones.

### Using vcpkg

Install [vcpkg](https://github.com/Microsoft/vcpkg) by cloning (or updating) the repository, and running the
bootstrapping script from the *vcpkg* directory:

    ./bootstrap-vcpkg.sh
    
Then, install packages as follows 

    ./vcpkg install libjpeg-turbo
    ./vcpkg install libpng
    
    ./vcpkg install opencv            # only for tests
    ./vcpkg install boost-filesystem  # only for tests
    ./vcpkg install benchmark         # only for benchmarks

(Windows users call `.\vcpkg.exe` instead of `./vcpkg`.)

Selene itself can also be [installed using *vcpkg*](installation.md).

### Using Conan

**Selene** supports optional use of [Conan](https://conan.io/) as a dependency manager on all supported platforms.
(It is also possible to install [a release of Selene itself](https://bintray.com/kmhofmann/conan-repo/selene%3Aselene)
using Conan).
See the full [Conan documentation](https://docs.conan.io/) for more information on how to use Conan.

Currently, `libjpeg-turbo`, `libpng` and `boost` (for the tests) can be built using Conan.
There are no stable, cross-platform Conan recipes available yet for `OpenCV` or Google's `benchmark` library.

To use Conan, first install it, e.g. with `pip`:

    pip install --user --upgrade conan

(On MacOS, you might prefer an installation with `brew` instead.)

The `conan-center` remote should be present with a default installation.
This can be checked by the `conan remote list` command.
If so, no further action is necessary; if not, add it with:

    conan remote add conan-center https://conan.bintray.com

From your (clean) build directory, call Conan before the CMake invocation (as follows, or similar):

    conan install .. --build missing
    
This will build the supported dependencies.
This step can take a while during its first call, but for future builds, all outputs should be cached.

By default, Conan builds static libraries.
If you want Conan to build shared libraries instead, add `-o *:shared=True` to the above Conan call.

Now you can invoke CMake as usual (see above), and it should find the respective Conan builds of the supported
dependencies.

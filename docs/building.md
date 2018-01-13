# Selene

[Main page](https://github.com/kmhofmann/selene)

## Building the library

### General

**Selene** uses [CMake](https://cmake.org/) for building.

Follow the usual instructions for CMake projects on UNIX-like systems.
First, clone the project and create a `build` directory (or use another name).

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build

Then call `cmake` and build the project. Using [GNU Make](https://www.gnu.org/software/make/):

    cmake -DSELENE_BUILD_TESTS=ON -DSELENE_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..
    make

Or, alternatively, using [ninja](https://ninja-build.org/):

    cmake -G Ninja -DSELENE_BUILD_TESTS=ON -DSELENE_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..
    ninja

On Windows, the CMake command might look similar to the following, in order to generate Visual Studio 2017 project
files for a 64-bit build (see below for more info on [vcpkg](https://github.com/Microsoft/vcpkg)):

    cmake -G "Visual Studio 15 2017 Win64" -T "host=x64" \
        -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>\scripts\buildsystems\vcpkg.cmake \
        -DSELENE_BUILD_TESTS=ON -DSELENE_BUILD_EXAMPLES=ON ..

Preferably use the library "at head", e.g. as submodule, instead of invoking the `install` target.

Integrating the library into own CMake projects can be as easy as:

    # ...
    add_subdirectory(selene)
    target_link_libraries(target_name selene::selene)
    # ...

The default settings will build a set of static libraries.
If you want to build shared libraries instead, add `-DBUILD_SHARED_LIBS=ON` to the `cmake` command. 

### Dependencies

**selene** uses the following (optional) third-party dependencies for implementing some of its functionality:

  - [libjpeg](http://www.ijg.org/) or [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) (preferred):
    - Optional, but recommended
    - Required for the JPEG reading and writing API
    - `libjpeg-turbo` is the preferred version of the library
  - [libpng](http://www.libpng.org/pub/png/libpng.html):
    - Optional, but recommended
    - Required for the PNG reading and writing API
  - [OpenCV](https://opencv.org/):
    - Optional, if needed
    - For OpenCV interoperability (e.g. copying or wrapping image data)

The presence (or lack of) these dependencies should be detected automatically by CMake.

If [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) is present on the system, some of its additional
capabilities (such as partial JPEG image decoding and extended color spaces) will be taken into account and are enabled
by the API.

[OpenCV](https://opencv.org/) is only needed for converting between `sln::Image<T>` and OpenCV's `cv::Mat` structure, if
so desired.

### Building and running the tests and examples

Building the tests is disabled by default, and can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake`
command.

The test suite depends on [Catch2](https://github.com/catchorg/Catch2) and [Boost.Filesystem](http://www.boost.org/) for
building the tests.
The former is bundled as a Git submodule and will be automatically cloned during execution of the `cmake` command.
The latter is automatically searched for by a CMake `find_package` command; its presence is required to build the tests.

The test suite can be run by executing `./test/selene_tests` from the `build` directory. `./test/selene_tests -h` lists
available options. See the [Catch2 documentation](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md)
for more information.

The repository also contains commented examples which can be optionally compiled.
This can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake` command; the examples can then be found in the
`./examples/` folder in the build directory.

In case some tests or examples are failing because auxiliary data files can not be found automatically, specify the path
to the `data` directory inside the `selene/` folder manually: `SELENE_DATA_PATH=../data ./test/selene_tests` (or
similar).

### Installing dependencies

The following are recommendations for installation of dependencies on various platforms.
Of course any dependency can alternatively also be built and installed from source, as long as it can be found by the
CMake `find_package` command.

#### Linux

On Debian-like systems (e.g. Ubuntu), you should be able to use `apt-get` as follows:

    apt-get install libjpeg-turbo8-dev libpng16-dev libopencv-dev libboost-filesystem-dev

However, OpenCV might need to be built from source and installed manually.

#### MacOS

Install [Homebrew](https://brew.sh/) to build and install the dependencies as follows:

    brew install libjpeg-turbo libpng opencv3 boost

#### Windows

By far the easiest way is to install and then use the [vcpkg](https://github.com/Microsoft/vcpkg) package manager:

    .\vcpkg.exe install libjpeg-turbo
    .\vcpkg.exe install libpng
    .\vcpkg.exe install opencv
    .\vcpkg.exe install boost

Set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` before installing the above packages to install
the 64-bit compiled versions instead of the 32-bit ones.
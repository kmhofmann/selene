# Selene

**Early preview release**

## Introduction

**selene** is a C++14 utility library for various image or IO related tasks.

It currently contains, among other things

  * Statically and dynamically typed in-memory pixel and image data representations.
  	* [`Pixel<T, n>`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Pixel.hpp): Statically typed multi-channel pixel value. For example, `Pixel<std:uint8_t, 3>` (shortcut: `Pixel_8u3`) can represent an 8-bit RGB value.
  	* [`Image<T>`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Image.hpp): Statically typed class representing a 2-D image. For example `Image<Pixel_8u1>` can represent an 8-bit grayscale image.
  	* [`ImageData`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageData.hpp): Dynamically typed class representing a 2-D image.
  	* Interoperability with [OpenCV](https://opencv.org/) `cv::Mat` matrices: both wrapping (as view) or copying is supported, in both directions. 
  * Functions for reading and writing image data in JPEG and PNG formats (from/to files or memory).
  	* [`read_jpeg()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp), [`read_jpeg_header()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp)
  	* [`read_png()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp), [`read_png_header()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp)
  	* [`write_jpeg()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGWrite.hpp)
  	* [`write_png()`](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGWrite.hpp)
  * Functions for binary IO from and to files or memory. The type of source/sink can be transparent to users of this functionality, via static polymorphism.
    * [`FileReader`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileReader.hpp)/[`FileWriter`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileWriter.hpp): Reading/writing from and to files
    * [`MemoryReader`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryReader.hpp)/[`MemoryWriter`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryWriter.hpp): Reading/writing from and to memory (raw pointer locations)
    * [`VectorReader`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorReader.hpp)/[`VectorWriter`](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorWriter.hpp): Reading/writing from and to `std::vector<std::uint8_t>`, extending as needed when writing
  * A simple thread pool, to enable task-based parallelism.
    * [`ThreadPool`](https://github.com/kmhofmann/selene/blob/master/src/selene/thread/ThreadPool.hpp): Concurrent processing of tasks (function calls).

## Installation

**selene** uses [CMake](https://cmake.org/) for building.

Installation targets are not yet supported, but might be added later.
Preferably use "at head" as submodule instead of installing.

Follow the usual instructions for CMake projects on UNIX-like systems.
First, clone the project and create a `build` directory (or use another name).

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build

Then call `cmake` and build the project. Using [GNU Make](https://www.gnu.org/software/make/):

    cmake -DCMAKE_BUILD_TYPE=Release .. && make

Or, alternatively, using [ninja](https://ninja-build.org/):

    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja

The default settings will build a set of static libraries.
If you want to build shared libraries instead, add `-DBUILD_SHARED_LIBS=ON` to the `cmake` command. 

For JPEG and PNG input/output, [libjpeg](http://www.ijg.org/) and [libpng](http://www.libpng.org/pub/png/libpng.html) are required, respectively, and their presence (or lack thereof) should be detected automatically.
If [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) is present on the system, some of its additional capabilities will be taken into account (e.g. partial JPEG image decoding and extended color spaces).

### Building and running the tests

The test suite depends on [Catch2](https://github.com/catchorg/Catch2) and [Boost.Filesystem](http://www.boost.org/) for building the tests.
The former is bundled as a Git submodule and will be automatically cloned during execution of the `cmake` command.

Building the tests is enabled by default, but can be disabled by adding `-DENABLE_SELENE_TESTS=OFF` to the `cmake` command. In this case, the `Boost.Filesystem` dependency disappears.

The test suite can be run by executing `./test/selene_tests` from the `build` directory. `./test/selene_tests -h` lists available options. See the [Catch2 documentation](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md) for more information.

In case some tests are failing because auxiliary data files can not be found automatically, specify the path to the `data` directory inside the `selene/` folder manually: `SELENE_DATA_PATH=../data ./test/selene_tests` (or similar).

## Documentation

**selene** is fully documented using the [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) format.
_Doxygen_ documentation can be built using the provided `Doxyfile`.

Currently, this documentation is not hosted online, although this may change in the future.

## Status

**selene** is work-in-progress and shall be considerably extended and modified in the future.
Currently, no API or ABI stability guarantees are given.

However, the code is well-tested and assumed to be not too far from production-level quality.
Porting to accommodate potential API changes should not be very difficult.


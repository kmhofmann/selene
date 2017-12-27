# Selene

**Early preview release**

## Introduction

**selene** is a C++14 utility library for various image or IO related tasks.

It currently contains, among other things

  * Statically and dynamically typed in-memory pixel and image data representations.
  	* [Pixel<T, n>](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Pixel.hpp): Statically typed multi-channel pixel value. For example, `Pixel<std:uint8_t, 3>` (shortcut: `Pixel_8u3`) can represent an 8-bit RGB value.
  	* [Image<T>](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Image.hpp): Statically typed class representing a 2-D image. For example `Image<Pixel_8u1>` can represent an 8-bit grayscale image.
  	* [ImageData](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageData.hpp): Dynamically typed class representing a 2-D image.
  	* Interoperability with [OpenCV](https://opencv.org/) `cv::Mat` matrices: both wrapping (as view) or copying is supported, in both directions. 
  * Functions for reading and writing image data in JPEG and PNG formats (from/to files or memory).
  	* [read_jpeg()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp), [read_jpeg_header()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGRead.hpp)
  	* [read_png()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp), [read_png_header()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGRead.hpp)
  	* [write_jpeg()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/JPEGWrite.hpp)
  	* [write_png()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PNGWrite.hpp)
  	* Convenience functions [read_image()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/IO.hpp) and [write_image()](https://github.com/kmhofmann/selene/blob/master/src/selene/img/IO.hpp), being able to handle both formats.
  	  * Example: `auto img_data = read_image(FileReader("image.png"));`
  	  * Example: `auto img_data = read_image(MemoryReader(data, size));`
  * Basic image processing functionality, such as:
    * Image [pixel access](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageAccess.hpp) using various [interpolation algorithms](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Interpolators.hpp) (nearest neighbor, bilinear) and [boundary handling strategies](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Accessors.hpp) (no check, replicate boundary, zero padding).
      * Example: `const auto x = get<ImageInterpolationMode::Bilinear>(img, 8.5, 10.2);`
    * [Algorithms](https://github.com/kmhofmann/selene/blob/master/src/selene/img/Algorithms.hpp) to apply pointwise operations to images/views.
      * Example: `for_each_pixel(img, [](auto& px){ px += 1; });`
    * [Pixel-level](https://github.com/kmhofmann/selene/blob/master/src/selene/img/PixelConversions.hpp) and [image-level](https://github.com/kmhofmann/selene/blob/master/src/selene/img/ImageConversions.hpp) conversion functions between different pixel formats (e.g. RGB -> Grayscale, etc.).
      * Example: `const auto gray_img = convert_image<PixelFormat::RGB, PixelFormat::Y>(rgb_img);`
  * Functions for binary IO from and to files or memory. The type of source/sink can be transparent to users of this functionality, via static polymorphism.
    * [FileReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileReader.hpp)/[FileWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/FileWriter.hpp): Reading/writing from and to files
    * [MemoryReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryReader.hpp)/[MemoryWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/MemoryWriter.hpp): Reading/writing from and to memory (raw pointer locations)
    * [VectorReader](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorReader.hpp)/[VectorWriter](https://github.com/kmhofmann/selene/blob/master/src/selene/io/VectorWriter.hpp): Reading/writing from and to `std::vector<std::uint8_t>`, extending as needed when writing
  * A simple thread pool, to enable task-based parallelism.
    * [ThreadPool](https://github.com/kmhofmann/selene/blob/master/src/selene/thread/ThreadPool.hpp): Concurrent processing of tasks (function calls).

## Installation

**selene** uses [CMake](https://cmake.org/) for building.

Follow the usual instructions for CMake projects on UNIX-like systems.
First, clone the project and create a `build` directory (or use another name).

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build

Then call `cmake` and build the project. Using [GNU Make](https://www.gnu.org/software/make/):

    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

Or, alternatively, using [ninja](https://ninja-build.org/):

    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
    ninja

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

  - [libjpeg](http://www.ijg.org/) or [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo):
    - Optional, but recommended
    - Required for the JPEG reading and writing API
  - [libpng](http://www.libpng.org/pub/png/libpng.html):
    - Optional, but recommended
    - Required for the PNG reading and writing API
  - [OpenCV](https://opencv.org/):
    - Optional, if needed
    - For OpenCV interoperability (e.g. copying or wrapping image data)

The presence (or lack of) these dependencies should be detected automatically by CMake.

If [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) is present on the system, some of its additional capabilities will be taken into account and are enabled by the API (e.g. partial JPEG image decoding and extended color spaces).

[OpenCV](https://opencv.org/) is only needed for converting between `sln::Image<T>` and OpenCV's `cv::Mat` structure, if so desired.

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

**selene** is currently developed and tested on Linux with GCC 7.2.0 and Clang 6.0.0 (trunk), and on MacOS with AppleClang 9.0.0.
Support for Windows/Visual C++ 2017 is under consideration, but not a priority.
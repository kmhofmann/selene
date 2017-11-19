# Selene

**Early preview release**

## Introduction

`selene` is a C++14 utility library for various image or IO related tasks.

It currently contains

  * Statically and dynamically typed in-memory pixel and image data representations.
  	* `Pixel<T, n>`: Statically typed multi-channel pixel value. For example, `Pixel<std:uint8_t, 3>` (shortcut: `Pixel_8u3`) can represent an 8-bit RGB value.
  	* `Image<T>`: Statically typed class representing a 2-D image. For example `Image<Pixel_8u1>` can represent an 8-bit grayscale image.
  	* `ImageData`: Dynamically typed class representing a 2-D image.
  * Functions for reading and writing image data in JPEG and PNG formats (from/to files or memory).
  	* `read_jpeg()`, `read_jpeg_header()`
  	* `read_png()`, `read_png_header()`
  * Functions for binary IO from and to files or memory. The type of source/sink can be transparent to users of this functionality, via static polymorphism.
    * `FileReader`/`FileWriter`: Reading/writing from and to files
    * `MemoryReader`/`MemoryWriter`: Reading/writing from and to memory (raw pointer locations)
    * `VectorReader`/`VectorWriter`: Reading/writing from and to `std::vector<std::uint8_t>`, extending as needed when writing
  * A simple thread pool, to enable task-based parallelism.
    * `ThreadPool`: Concurrent processing of tasks (function calls).

## Installation

**TODO: More detailed installation instructions.**

`selene` uses CMake for building.

Installation targets are not yet supported, but might be added later.
Preferably use "at head" as submodule instead of installing.

Follow the usual instructions for CMake projects on UNIX-like systems:

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja    # alternatively use old-school make

The default settings will build a set of static libraries.
If you want to build shared libraries instead, add `-DBUILD_SHARED_LIBS=ON` to the `cmake` command.
(You might have to add `-DSELENE_SEARCH_LIBJPEGTURBO=OFF`, if shared libraries of `libjpeg-turbo` are not distributed on your system.) 

For JPEG and PNG input/output, `libjpeg` [(ijg.org)](http://www.ijg.org/) and `libpng` [(libpng.org)](http://www.libpng.org/pub/png/libpng.html) are required, respectively, and their presence (or lack thereof) should be detected automatically. If `libjpeg-turbo` [(GitHub)](https://github.com/libjpeg-turbo/libjpeg-turbo) is present on the system, it will be chosen over `libjpeg`.

The test suite depends on `Catch2` [(GitHub)](https://github.com/catchorg/Catch2) and `Boost.Filesystem` [(boost.org)](http://www.boost.org/) for building the tests.
The former is bundled as a Git submodule and will be cloned during execution of the `cmake` command.

Building the tests can be disabled by adding `-DENABLE_SELENE_TESTS=OFF` to the `cmake` command; then the `Boost.Filesystem` dependency disappears.



## Documentation

Unfortunately, no complete documentation exists yet.
Some functions are documented using the _Doxygen_ format, but this is still heavy work in progress.
There is no point in building _Doxygen_ documentation yet.

For the time being, the public interfaces in the header files shall serve as (mostly self-documenting) documentation.

## Status

`selene` is work-in-progress and shall be considerably extended and modified in the future.
Currently, no API or ABI stability guarantees are given.

However, the code is well-tested and assumed to be not too far from production-level quality.
Porting to accommodate potential API changes should not be very difficult.


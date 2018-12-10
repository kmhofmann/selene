# Selene

## Building the library

### General

**Selene** requires a compiler supporting C++17 and uses [CMake](https://cmake.org/) for building.

See the [Dependencies](dependencies.md) page for more information on its required and optional dependencies.
These need to be installed before attempting to build the respective parts.

Follow the usual instructions for CMake projects on UNIX-like systems.
First, clone the project and create a `build` directory (or use another name).

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build

Then call `cmake` and build the project.

    cmake -DCMAKE_BUILD_TYPE=Release -DSELENE_BUILD_ALL=ON ..
    cmake --build .      # or just 'make', if GNU Make is the generator

Additional options may be passed to the respective build tool, e.g. `cmake --build . -- -j8`.

On Windows, the CMake command might look similar to the following, in order to generate Visual Studio 2017 project
files for a 64-bit build (see [here](dependencies.md) for more info on using 
[vcpkg](https://github.com/Microsoft/vcpkg) for installation of dependencies):

    cmake -G "Visual Studio 15 2017 Win64" -T "host=x64" \
        -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>\scripts\buildsystems\vcpkg.cmake \
        -DSELENE_BUILD_ALL=ON \
        ..

The setting `-DSELENE_BUILD_ALL=ON` enables building the tests, examples, and benchmarks.
Omit this parameter if this is not desired, and see below for the individual CMake variables that can be set for
more fine-grained control.

#### Static vs. shared libraries

The default CMake settings will build a set of static libraries.

If you want to build shared libraries instead, add `-DBUILD_SHARED_LIBS=ON` to the `cmake` command.

### Installation/Usage

* **Selene** can be installed using the following `cmake` invocation:

      cmake --build . --target install

  Alternatively, the respective command provided by the build system can be used; e.g. `make install` for GNU Make,
or `ninja install` for ninja.

  Selene can then be found by other CMake projects using the `find_package` command:
  
        find_package(selene)
        # ...
        target_link_libraries(<target_name> selene::selene)

* By default, above installation will be system-wide (e.g. to `/usr/local`) and likely requires administrator rights.
  Since this is an intrusive operation, and may result in file level conflicts (due to the single directory for all
  libraries and applications), it is generally *not* recommended.
  
  To perform a user-local installation instead, change the CMake prefix path by adding
  `-DCMAKE_INSTALL_PREFIX=<your_custom_location>` to the *initial* CMake invocation.
  For example: `cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local/selene ..`.

  Note how this approach can easily provide a much cleaner separation of library/application files on a
  library/application level on the filesystem.

* Even *without* explicit installation, CMake adds a reference to the build tree location to the user-level CMake cache.
This means that the above `find_package()` call will also work without user-level or system-level installation, and will
then find the build tree itself.
**Selene** just needs to have been successfully built. 

* Another option is to keep the library as a submodule within your project.
  This enables Selene to be built from source *together* with your project, i.e. enabling a true "one-command" build.
  However, you won't be able to use the idiomatic `find_package` command, but need to use `add_subdirectory`
  instead:

      add_subdirectory(selene)  # assuming the library is cloned as submodule in a directory named 'selene'
      # ...
      target_link_libraries(<target_name> selene::selene)

  Advantages of this approach are greatly decreased risk of inconsistent dependencies (in case you upgrade libraries),
  and IDEs more easily picking up the Selene source code (as opposed to, say, just the installed headers).
  The main disadvantage is the non-idiomatic approach to package management (or, rather, the lack of it).

### CMake options

**Selene** provides a number of options that can be either passed to the `cmake` command line invocation, or
alternatively set in a helper tool like `ccmake`.

#### Building everything

    -DSELENE_BUILD_ALL=ON
    
This is a convenience option to enable building tests, examples, and benchmarks.
More fine-grained control can be achieved by the options right below.

#### Building tests

    -DSELENE_BUILD_TESTS=ON

Building the tests is disabled by default, and can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake`
command.

The test suite depends on [Catch2](https://github.com/catchorg/Catch2) and [Boost.Filesystem](http://www.boost.org/) for building the tests.
The former is bundled as a Git submodule and will be automatically cloned during execution of the `cmake` command.
The latter is automatically searched for by a CMake `find_package` command; its presence is required to build the tests.

The test suite can be run by executing `./test/selene_tests` from the `build` directory. `./test/selene_tests -h` lists
available options. See the [Catch2 documentation](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md)
for more information.

#### Building examples

    -DSELENE_BUILD_EXAMPLES=ON

The repository also contains commented examples which can be optionally compiled.
This can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake` command.
The examples can then be found in the `./examples/` folder in the build directory.

#### Building benchmarks

    -DSELENE_BUILD_BENCHMARKS=ON

A few micro-benchmarks can be optionally compiled by adding `-DSELENE_BUILD_BENCHMARKS=ON` to the `cmake` command line.
The code for these can be found in the `./benchmark/` folder, and depends on Google's
[benchmark](https://github.com/google/benchmark) library to be installed.

#### Excluding dependencies

By default, the **Selene** CMake build instructions will automatically detect library [dependencies](dependencies.md)
present on the system and make use of these.
If desired, this can be explicitly disabled by one or more of the following CMake options:

    -DSELENE_NO_LIBJPEG
    -DSELENE_NO_LIBPNG
    -DSELENE_NO_OPENCV

### Running tests & examples

#### Specifying the data path

In case some tests or examples are failing because auxiliary data files can not be found automatically, specify the path
to the `data` directory inside the `selene/` folder manually: `SELENE_DATA_PATH=../data ./test/selene_tests` (or
similar).

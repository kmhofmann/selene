# Selene

* [Building the library using CMake](#section_cmake)
* [Building the library using the `easy_build` script](#section_easy_build)

## <a name="section_cmake"></a>Building the library using CMake

**Selene** requires a compiler supporting C++17 and uses [CMake](https://cmake.org/) for building.

See the [Dependencies](dependencies.md) page for more information on its required and optional dependencies.
These need to be installed before attempting to build the respective parts.

### General

Follow the usual instructions for CMake projects on UNIX-like systems (**Linux**, **macOS**, **Windows Subsystem for Linux**).

First, clone the project and create a `build` directory (or use another name).

    git clone https://github.com/kmhofmann/selene.git
    cd selene
    mkdir build && cd build

Then call `cmake` and build the project, e.g. with.

    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DSELENE_BUILD_ALL=ON ..
    cmake --build . -j           # Or just 'make -j[N]', if GNU Make is the generator;
                                 # or 'ninja', if Ninja is the generator.

Additional options may be passed to the respective build tool, e.g. `cmake --build . -- -j8`.

The setting `-DSELENE_BUILD_ALL=ON` enables building the tests, examples, and benchmarks.
Omit this parameter if this is not desired, and see below for the individual CMake variables that can be set for
more fine-grained control.

On **Windows**, recent versions of Visual Studio support opening CMake-based projects directly from the IDE.
Additional variables (e.g. `-DSELENE_BUILD_ALL=ON`) can be specified through a somewhat unintuitive JSON text file. 
 
Otherwise, CMake can be used to generate Visual Studio project files manually.
The CMake command might look similar to the following, in order to generate project VS2017 files for a 64-bit build
(see [here](dependencies.md) for more info on using [vcpkg](https://github.com/Microsoft/vcpkg) for installation of
dependencies):

    cmake -G "Visual Studio 15 2017 Win64" -T "host=x64" \
        -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>\scripts\buildsystems\vcpkg.cmake \
        -DSELENE_BUILD_ALL=ON \
        ..

#### Static vs. shared libraries

The default CMake settings will build a set of static libraries.

If you want to build shared libraries instead, add `-DBUILD_SHARED_LIBS=ON` to the `cmake` command.

### Installation/Usage

* **Selene** can be installed using the following `cmake` invocation:

      cmake --build . --target install

  Alternatively, the respective command provided by the build system can be used; e.g. `make install` for GNU Make,
or `ninja install` for [Ninja](https://ninja-build.org/).

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

  Note how this approach can provide a much cleaner separation of library/application files on a
  library/application level.

* Even *without* explicit installation, CMake adds a reference to the build tree location to the user-level CMake cache.
This means that **the above `find_package()` call will also work without user-level or system-level installation**, and
will then find the build tree itself.
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
It implies `-DSELENE_BUILD_TESTS=ON -DSELENE_BUILD_EXAMPLES=ON -DSELENE_BUILD_BENCHMARKS=ON`.
More fine-grained control can be achieved by using the options right below.

#### Building tests

    -DSELENE_BUILD_TESTS=ON

Building the tests is disabled by default, and can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake`
command.

#### Building examples

    -DSELENE_BUILD_EXAMPLES=ON

The repository also contains commented examples which can be optionally compiled.
This can be enabled by adding `-DSELENE_BUILD_TESTS=ON` to the `cmake` command.
The examples can then be found in the `./examples/` folder in the build directory.

#### Building benchmarks

    -DSELENE_BUILD_BENCHMARKS=ON

A few micro-benchmarks can be optionally compiled by adding `-DSELENE_BUILD_BENCHMARKS=ON` to the `cmake` command line.
The code for these can be found in the `./benchmark/` folder.

#### Excluding dependencies

By default, the **Selene** CMake build instructions will automatically detect library [dependencies](dependencies.md)
present on the system and make use of these.
If desired, this can be explicitly disabled by one or more of the following CMake options:

    -DSELENE_USE_LIBJPEG=OFF
    -DSELENE_USE_LIBPNG=OFF
    -DSELENE_USE_LIBTIFF=OFF
    -DSELENE_USE_OPENCV=OFF

The respective functionality, i.e. image I/O, or interoperability with OpenCV's `cv::Mat`, will then be disabled.

### Running tests & examples

The tests can be run by executing `./test/selene_tests` from the build directory.
Execute it as follows to see an overview of the performed tests while running:

    ./test/selene_tests -d yes
    
`./test/selene_tests -h` lists other available options.
See the [Catch2 documentation](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md)
for more information.

#### Specifying the data path

In case some tests or examples are failing because auxiliary data files can not be found automatically, specify the path
to the `data` directory inside the `selene/` folder manually: `SELENE_DATA_PATH=../data ./test/selene_tests` (or
similar).

<hr>

## <a name="section_easy_build"></a>Building the library using the `easy_build` script

On Linux systems, one additional option to build **Selene** is via the `scripts/easy_build.sh` *bash* script.

Its main purpose is to provide a proof-of-concept for building the library on older (Ubuntu) Linux systems, and to make
this process relatively straightforward.
As such, it tries to rely on as few system packages as possible.

**Generally, the above [*CMake* based](#section_cmake) "manual" building approach is preferable**, but it assumes that
recent enough versions of *CMake* and the compiler are installed, and that all dependencies are present as well in an ABI
compatible form.

The `easy_build` script will build all parts of Selene, i.e. the libraries (as static libraries), tests, examples
and benchmarks.
All necessary dependencies will be built and installed (as static libraries) through a local clone of
[vcpkg](https://github.com/Microsoft/vcpkg).

It will also:
* Download a recent enough version of CMake to use for build file generation.
* Check whether the default compiler supports the required features (i.e. C++17).
  If not, it provides the option (on Ubuntu) to download a recent version of Clang, and to build **Selene** using
  Clang/libc++.
* Run the unit tests, if desired.

The script does not provide any configuration choices as described above; all CMake options are enabled.

From the project's root directory, call

    ./scripts/easy_build.sh 
    
to get an overview over its options.
To actually trigger the build process, add `-X` as an option, and follow potential instructions.

Notes:
* `git`, `curl`, `wget`, `unzip` and `tar` are required to be present on the system.
* On Ubuntu 14.04, a newer version of `git` needs to be installed via the official [upstream PPA](https://git-scm.com/download/linux).
* *vcpkg* requires building with a reasonably new GCC version, and will display its own instructions how to install this.

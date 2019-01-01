# Selene

## Installation

Besides building from source, **Selene** can be installed using popular C++ package managers on Linux, macOS and Windows.

### Building from source

Building the library from source is the recommended way to use **Selene**.

See [here](building.md) for detailed instructions.

### Using vcpkg

Install [vcpkg](https://github.com/Microsoft/vcpkg) by cloning (or updating) the repository, and running the
bootstrapping script from the *vcpkg* directory:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh

Please ensure that your installed version of *vcpkg* is up to date.
Then, the most recent release of **Selene** (that is represented in the *vcpkg* repository) can be installed with:

    ./vcpkg install selene

Alternatively, in order to build and install from `HEAD`, use:

    ./vcpkg install --head selene

To make use of the *vcpkg*-installed version of Selene, please refer to the [vcpkg documentation](https://vcpkg.readthedocs.io/).
Using CMake, this is as easy as specifying the respective CMake Toolchain file during the `cmake` invocation:

     ... -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake ...


### Using Conan

To use Conan, first install it, e.g. with `pip`:

    pip install --user --upgrade conan

(On MacOS, you might prefer an installation with `brew` instead.)

Assuming that [Conan](https://conan.io/) is set up on the target machine, the first step is to set up the remote
used to host the **Selene** package:

    conan remote add kmhofmann https://api.bintray.com/conan/kmhofmann/conan-repo

This has to be done once per Conan installation.
Then install the actual package; the current library version provided is `0.2`.

    conan install selene/0.2@selene/testing -r kmhofmann

Alternatively, just specify `selene/0.2@selene/testing` in a project's `conanfile.txt`.

Please refer to the [Conan documentation](http://docs.conan.io/) on how to make use of the installed package. 
See also the Bintray website for Selene: https://bintray.com/kmhofmann/conan-repo/selene%3Aselene.
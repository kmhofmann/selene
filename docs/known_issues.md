# Selene

## Known issues

* When using [Conan](https://conan.io/) to provide `libjpeg-turbo` as a static library on Windows, the library might not
be found by CMake.

  This is because the binary library file is called `jpeg-static.lib`, but CMake is not looking for a file with a
  `-static` suffix.
  
  This will be fixed in CMake >3.12.0; see
  [this pull request](https://gitlab.kitware.com/cmake/cmake/merge_requests/2059), but an possible interim workaround
  is to change the line
  
      set(jpeg_names ${JPEG_NAMES} jpeg libjpeg)

  to
  
      set(jpeg_names ${JPEG_NAMES} jpeg jpeg-static libjpeg)

  in `FindJPEG.cmake`.
  This file can be found in the global location where CMake stores its module files (i.e. as part of
  the CMake installation).
  
* When using [vcpkg](https://github.com/Microsoft/vcpkg) to provide `libpng` on Linux, the library might not be
correctly found by CMake.

  This is because the binary library file is called `libpng16.a`, but CMake is first looking for library files called
  `libpng.*`, which includes e.g. `libpng.so` from the system-provided package in Ubuntu.
  The resulting situation is that the headers are coming from the `vcpkg` directory, but the library is not.
  
  See [this pull request](https://gitlab.kitware.com/cmake/cmake/merge_requests/2094).
  An interim workaround could be to change `FindPNG.cmake` in the CMake modules directory as
  [in this commit](https://gitlab.kitware.com/cmake/cmake/commit/ac84eabefc05d155d14c9b626f4abc2e62b8e37a).  
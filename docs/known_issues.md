# Selene

## Known issues

* When using [Conan](https://conan.io/) to build `libjpeg-turbo` as a static library on Windows, the library might not
be found by CMake.

  This is because the binary library file is called `jpeg-static.lib`, but CMake is not looking for a file with a
  `-static` suffix.
  This may be fixed future CMake versions via
  [this pull request](https://gitlab.kitware.com/cmake/cmake/merge_requests/2059), but an possible interim workaround
  is to change the line
  
      set(jpeg_names ${JPEG_NAMES} jpeg libjpeg)

  to
  
      set(jpeg_names ${JPEG_NAMES} jpeg jpeg-static libjpeg)

  in `FindJPEG.cmake`.
  This file can be found in the global location where CMake stores its module files (i.e. as part of
  the CMake installation).
  
  (This issue does not occur when using [vcpkg](https://github.com/Microsoft/vcpkg) as package manager, as its
  statically built library is renamed to `jpeg.lib`.)
# Selene

## Using the library in other projects

This page describes how to use **Selene** as a dependency in other projects.

* [Using CMake](#section_cmake)
* [Using other build systems](#section_other_build_systems)
* ["I just want to dump the code into my source tree."](#section_dump_code)

### <a name="section_cmake"></a>Using CMake

#### Using `find_package` (recommended)

After building or installing Selene, the library can then be used from other CMake projects via the
`find_package` command.

    # ...
    find_package(selene)
    target_link_libraries(<target_name> selene::selene)
    # ...

The following targets are provided, with dependencies between the targets being automatically resolved:

* `selene::selene` &mdash; an interface target for all below targets
* `selene::selene_base` &mdash; base components
* `selene::selene_base_io` &mdash; general I/O routines
* `selene::selene_img` &mdash; image &amp; pixel representation
* `selene::selene_img_ops` &mdash; image operations
* `selene::selene_img_io` &mdash; general image I/O routines
* `selene::selene_img_io_jpeg` &mdash; image I/O routines for JPEG format (if built with JPEG support)
* `selene::selene_img_io_png` &mdash; image I/O routines for PNG format (if built with PNG support)
* `selene::selene_img_io_tiff` &mdash; image I/O routines for TIFF format (if built with TIFF support)

Usually, only the target `selene::selene` is needed to be specified, unless one wants to specifically exclude
sub-components of the library.

For all targets, include and library paths will be automatically added, so **no** explicit calls to
`target_include_directories`, `include_directories`, or `link_directories` are needed for the purpose of adding
**Selene** as a dependency.

#### Using `add_subdirectory`

Another option is to keep the library as a [Git] submodule within your project.
This enables Selene to be built from source *together* with your project, i.e. enabling a true "one-command" build.
However, you won't be able to use the idiomatic `find_package` command, but need to use `add_subdirectory`
instead:

    add_subdirectory(selene)  # assuming the library is cloned as submodule in a directory named 'selene'
    target_link_libraries(<target_name> selene)

Advantages of this approach are decreased risk of inconsistent dependencies (in case you upgrade libraries),
and IDEs more easily picking up the Selene source code (as opposed to, say, just the installed headers).
The main disadvantage is the non-idiomatic approach to package management (or, rather, the lack of it).


### <a name="section_other_build_systems"></a>Using other build systems

**Selene** does not automatically integrate with other build systems.
However, after installation of **Selene** using CMake (as described [here](installation.md)), one can quite
easily enable usage of Selene inside other projects by:

1) Adding the correct include path, `[INSTALL_PREFIX]/include`;
2) Adding the respective libraries, located in `[INSTALL_PREFIX]/lib`;
3) Ensuring that all required dependencies (that **Selene** was built with) can be found.

where `[INSTALL_PREFIX]` is the path to the installation location.

Integrating the output from the build process (i.e. the build tree output, prior to a potential installation step)
while not recommended, is not that difficult or tricky either:

1) Add the include paths `[SOURCE_LOCATION]/selene` and `[BUILD_PATH]/selene`.
2) The libraries are located in `[BUILD_PATH]/selene`.

where `[SOURCE_LOCATION]` is the located where the **Selene** source code was checked out to, and `[BUILD_PATH]`
is the path where the CMake build was performed in.


### <a name="section_dump_code"></a>"I just want to dump the code into my source tree."

This approach is not recommended. Really. Use CMake to build and install. It's a great tool.

For everyone who is unconvinced (and who thinks they know what they're doing), the following information
might be useful.

* **Selene** expects a configuration header called `selene_config.hpp` in its include path.
  This file is auto-generated during the `cmake` invocation, and is then located inside the `selene` sub-directory
  in the build directory.
  During the installation process, it is copied to the include path.
  
* This header can also be manually "generated" by inspecting the file `cmake/selene_config.hpp.in`, copying this
  file to the include path, and transforming the required `#cmakedefine`s into `#define`s, while leaving
  the rest undefined.

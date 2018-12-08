# How to contribute

An evolving contribution guide...

Before contributing, please have a look at the [Code of Conduct](CODE_OF_CONDUCT.md).
By contributing, you are expected to uphold this code.

## Contact

* The project lead and main author is reachable on the [Cpplang Slack channel](https://cpplang.slack.com/),
user name `michaelhofmann`.

## Reporting bugs

* The easiest way to report a bug is to simply [file an issue](https://github.com/kmhofmann/selene/issues/new)
here on GitHub.
* Before doing so, please ensure the bug was not already reported by searching on GitHub under
[Issues](https://github.com/kmhofmann/selene/issues).
* Be sure to include a title and clear description in the issue.
* Provide as much relevant information as possible. In particular, state what the expected behavior should be, and add
some sample code or a full test case that demonstrate that the expected behavior is not occurring.

## Pull requests

* Thank you for contributing! Read more about pull requests [here](https://help.github.com/articles/about-pull-requests/).
* Before sending or starting work on larger non-bugfix pull requests, i.e. pull requests that would add new features,
please contact the project lead first (see above) to align with the project goals.
This will greatly increase the chances of your pull request being accepted. 
* In a pull request, please provide a clear list of what you have done.
Always write a clear log message for your commits.
Include the relevant issue number, if applicable.
* Each pull request should have one clear function (i.e. adding a specific feature or fixing a particular bug).
Tests are part of this function and should always be provided.
However, separate concerns and keep pull requests at fine granularity.
 For example, do not submit several feature additions and/or bug fixes in one pull request.

## Feature requests

* At the moment, the project does not have a public roadmap, as it currently just has one major contributor working on
it during his spare time. 
* Please get in touch with the project lead (see above) to discuss feasibility and planning.
* The best feature request is provided in the form of a pull request. :-)

## Coding conventions

* Please adhere to the existing formatting conventions for reasons of consistency.
* The easiest way to adhere to the formatting conventions is to reformat your code using
[clang-format](https://clang.llvm.org/docs/ClangFormat.html) before committing.
A respective `.clang-format` file is [provided](.clang-format) in the repository, and (in absence of explicit
guidelines) serves as implicit documentation for the formatting conventions.
* General coding guidelines and conventions that this project follows are given
[here](https://github.com/kmhofmann/cpp_coding_guidelines).
* All code should be written in conforming, platform-independent C++17.
* The project should always compile using GCC (version 8+), Clang (version 7+), and the Visual C++ compiler
(recent version of Visual Studio 2017+).
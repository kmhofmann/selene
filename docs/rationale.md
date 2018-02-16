# Selene

[Main page](https://github.com/kmhofmann/selene)

## Rationale

#### Why a new library?

Why implement a library for image representation, I/O and basic processing?
It seems that popular existing solutions are somewhat lacking.

##### OpenCV's shortcomings

Many C++ developers who quickly want to read or write image data for processing purposes, in particular in research,
fall back to using [OpenCV](https://opencv.org/).

OpenCV, however, is a heavy-weight solution from a binary size (and functionality) perspective, and its main development
focus seems to be on adding new features instead of properly rewriting its inner core parts.

This means users still have to deal with limited and notoriously type-unsafe interfaces that to not generalize
well and provide few customization points.
The core library has heaps of scaffolding and questionable abstractions to hide C code from 20 years ago.
Examples:

- Accessing a pixel inside a `cv::Mat` instance correctly is error prone, as the image data type has to be decided at
each point of access and is not trivial to determine from a read image. Making a mistake here is easy.
- Many function like to take, for example, a `cv::InputArray` or a `cv::InputArrayOfArrays` as input.
These are actually the same type, masking an abstraction with a `void*` somewhere at the end of it.
Good-bye type safety!

##### libjpeg's/libpng's non-typesafe interfaces

Another option for C++ developers to read or write image data is to use the reference implementations for common formats
directly; e.g. [libjpeg](http://www.ijg.org/) or [libpng](http://www.libpng.org/pub/png/libpng.html).

This is also notoriously difficult to get right, because although these libraries are very well documented, their C
interfaces are very low-level, require lots of boilerplate code, and provide even less type safety.
Error handling even needs to be implemented using `setjmp` and `longjmp`!

And even if one gets all this right, the result is still a block of memory with decoded image data; there is no unifying
image representation class.

##### GIL - unmaintained

Other C++ image representation libraries include GIL, available either
[from Adobe](https://stlab.adobe.com/gil/) or as part of [Boost](https://www.boost.org/).

GIL is quite well designed and shares many of its design goals with **Selene**.
It distinguishes between images and views as separate types.
This requires a lot of templated code, if a function should be agnostic to whether an image representation is a
view or owns its data. **Selene**, on the other hand, has one (statically typed) image type that can be either (at the
expense of one byte per instantiation).

GIL offers image I/O routines only as an extension; overall, the reading and writing routines are quite rudimentary
and do not allow for any finer control of parameters.
Reading and writing from/to memory also does not seem to be supported.

Most problematically, the last update to GIL was [made in 2007]](https://stlab.adobe.com/gil/). It is quite fair to
assume that the code base in not maintained anymore. This means that no new features or bug fixes are likely to be
developed.

#### What does Selene aim to provide?

**Selene** aims to fill this gap by providing **lightweight components** with an **easy to use API** for the most common image I/O and processing needs.
See also the [feature set](overview.md).

Its goal is to provide a wide-enough feature set to be able to _build_ more complex functionality _on top_.

As a starting point, the initial feature set includes
- type-safe and customizable representation of image data
- basic image processing operations, such as color channel conversions
- flexible I/O operations for the most common formats (currently: JPEG & PNG) 

It is not intended to become a competitor to, say, OpenCV's more specialized computer vision algorithms, and it does
not aim to be a jack of all trades, master of none.

Primary design criteria for the library are:
- Ease of use ("easy to use correctly, hard to use incorrectly")
- Minimal and modern interfaces
- Modularity and composability
- Type safety

While performance is deemed important, and premature pessimization is avoided wherever possible, some of the provided
operations could be further sped up by explicit vectorization. As the library currently has one main author working on
it in his free time, this is deemed out of scope at the moment.
# Selene

## Rationale

### Why a new library?

Why implement a library for image representation, I/O and basic processing?
It seems that other popular existing solutions are somewhat lacking.
See below.

### What does Selene aim to provide?

**Selene** aims to fill this gap by providing **lightweight components** with an **easy to use API** for the most
common image I/O and processing needs.
See also the [feature set](overview.md).

Its goal is to provide a wide-enough feature set to be able to _build_ more complex functionality _on top_.

As a starting point, the initial feature set includes
- Type-safe and customizable representation of image data
- Basic image processing operations, such as color channel conversions
- Flexible I/O operations for the most common formats (currently: JPEG, PNG, TIFF) 

It is not intended to become a competitor to, say, OpenCV's more specialized computer vision algorithms, and it does
not aim to be a jack of all trades, master of none.

Primary design criteria for the library are:
- Ease of use ("easy to use correctly, hard to use incorrectly")
- Minimal and modern interfaces
- Modularity and composability
- Type safety

### Other libraries

#### OpenCV's shortcomings

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

#### libjpeg's/libpng's/libtiff's non-typesafe interfaces

Another option for C++ developers to read or write image data is to use the reference implementations for common formats
directly; e.g. [libjpeg](http://www.ijg.org/), [libpng](http://www.libpng.org/pub/png/libpng.html), or
[libtiff](http://www.simplesystems.org/libtiff/).

This is also notoriously difficult to get right, because although these libraries are very well documented
(*libtiff* excluded!), their C interfaces are very low-level, require lots of boilerplate code, and provide even
less type safety.
In some of these, error handling even needs to be implemented using `setjmp` and `longjmp`!

And even if one gets all this right, the result is still a block of memory with decoded image data; there is no unifying
image representation class.

#### GIL - unmaintained (until recently) &amp; Boost dependencies

Another quite interesting C++ image representation library is GIL, available either
[from Adobe](https://stlab.adobe.com/gil/) or as part of [Boost](https://www.boost.org/).

GIL is quite well designed and shares many of its design goals with **Selene**.
The library provides the concept of lazily evaluated views, which can be more efficient than what **Selene** currently
provides, depending on the application.
However, the maintenance situation around GIL is confusing.

(Adobe) GIL offers image I/O routines as an extension.
Overall, the reading and writing routines are quite rudimentary and do not allow for any finer control of parameters.
Reading and writing from/to memory also does not seem to be supported.
More problematically, the last update to GIL by the original implementer Adobe was
[made in 2007](https://stlab.adobe.com/gil/).

Maintenance of the code base as part of Boost, i.e. "Boost.GIL" has only been recently picked up by a single maintainer.
See the [repository on GitHub](https://github.com/boostorg/gil).

Boost.GIL is written in C++11, as opposed to a more modern standard version, and depends on many other Boost libraries.
Depending on the Boost usage policy, this may be quite undesirable.
**Selene does not depend on any Boost library**.

#### CImg - a really big mess

[CImg](http://cimg.eu/), the “C++ Template Image Processing Toolkit”, is advertised as a small, useful, generic, and
simple library.
In practice, it is unfortunately anything but.

In the bigger picture of things, it seems just a detail that CImg only supports planar data storage (according to its
[documentation](http://cimg.eu/reference/group__cimg__storage.html)), while **Selene** by default assumes
interleaved storage.
Selene currently supports multiple image planes via separate single-channel images/views only; this may change in the
future.
CImg does not seem to support interleaved storage at all.

More importantly, from a software engineering point of view, Selene is quite antithetical to CImg:
* In CImg, *all* operations on an image are modeled as member functions inside its own image class; the class has
literally hundreds of member functions.
In Selene, classes are used only to represent either data holding entities or views, and algorithms/operations on
images are modeled as free functions operating on these.
* The CImg code base, which is contained in a single [header file](https://framagit.org/dtschump/CImg/raw/master/CImg.h)
of almost 3MB, contains image representation &amp; I/O, but also window, mouse, and event handling routines, hard-coded
key codes, a least one hard-coded font set, a math formula parser and expression evaluator, many, many macros, string
handling and math routines, and who knows what.
* The main CImg author has made many such peculiar design choices, which seem to not follow good software engineering
practices.
The CImg entities exhibit very strong coupling and very low cohesion.
It becomes extremely difficult to reason about the design, modularity, and correctness of CImg.
The latter is greatly exacerbated by CImg not having *any* unit tests, as far as it seems.
* In contrast, the **Selene** code base is structured much more modularly.
Each header file only contains functionality of one single component, and header files follow a strict ‘include only
what you use' pattern.
Overall, strong attempts are made in the design to minimize coupling and maximize cohesion.

Besides all of these issues, CImg is licensed under two niche copyleft licenses, CeCILL and CeCILL-C, roughly comparable
to GNU GPL and GNU LGPL.
Either of these licenses often results in greatly decreased adoption by industry.
In contrast, **Selene** is distributed under the more permissive MIT license.

#### stb - low-level C re-implementations of I/O routines; partial format support

[stb](https://github.com/nothings/stb) is a collection of header-only C libraries for various purposes.
Among these purposes, two header files enable image reading for a variety of formats (JPEG and PNG included, but
not TIFF), or writing for a few formats (only PNG, TGA, BMP).
These header files contain complete re-implementations of I/O routines for the supported formats.
Many of these formats are only partially supported, and the I/O routines are only meant for particular use cases, e.g.
for known, controlled image data.

No attempt is made in *stb* at providing an abstraction toward coherent image representation or access;
in every case, the image data is delivered or has to be supplied as a block of raw memory.
With the implementation done in pure C, and without relying on other, external libraries, *stb_image.h*'s code stays
on a very low level, without expressing its functionality using any form of higher level abstraction.
This makes it quite difficult to reason about its correctness, but also potentially erodes the trust in the library.

Several other image format I/O reference implementations (e.g. *libjpeg*/*libpng*/*libtiff*) exhibit the same
low-level characteristics, and **Selene** depends on them for their respective purposes, but these reference
implementations seem to be far more widely used and battle-tested than *stb* is.

It may be interpreted as positive that all of *stb* is in the public domain.
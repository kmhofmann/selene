// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

// Purely for Doxygen documentation purposes.

/// @file

/*! \mainpage Selene API documentation
 *
 * **Selene** is a C++17 image representation, processing and I/O library.
 *
 * For introductory information about the library, please visit its GitHub page:
 * - https://github.com/kmhofmann/selene
 *
 * It currently contains, among other things:
 * - Statically and dynamically typed in-memory image data representations.
 * - Functions for reading and writing image data in JPEG, PNG, and TIFF formats (from/to files or memory).
 * - Basic image processing functionality, such as:
 *   - Image pixel access using various interpolation algorithms (nearest neighbor, bilinear) and boundary handling
 * strategies (no check, replicate boundary, zero padding).
 *   - Algorithms to apply point-wise operations to images/views.
 *   - Pixel-level and image-level conversion functions between different pixel formats (e.g. RGB -> Grayscale, etc.).
 *   - Convolution using 1D kernels
 *   - Image resampling
 * - Functions for interoperability with OpenCV cv::Mat matrices (wrapping or cloning).
 * - Functions for binary IO from and to files or memory.
 *
 * This is the automatically generated Doxygen documentation for the **Selene** API.
 */

/// \defgroup group-base Base
/// Base functionality and classes.

/// \defgroup group-base-io Binary I/O
/// Binary I/O functionality and classes.



/// \defgroup group-img-pixel Pixel representation
/// Pixel representation class, traits.

/// \defgroup group-img-pixel-alias Pixel type aliases
/// Pixel type aliases.

/// \defgroup group-img-common Image representation (common functionality)
/// Common image representation functionality, classes and concepts.

/// \defgroup group-img-dynamic Image representation (dynamic pixel type)
/// Image representation for images with dynamic (runtime determined) pixel type.

/// \defgroup group-img-typed Image representation (static pixel type)
/// Image representation for images with static (compile-time determined) pixel type.

/// \defgroup group-img-typed-alias Typed image type aliases
/// Typed image type aliases.

/// \defgroup group-img-interop Image representation interoperability
/// Image representation interoperability between dynamic/typed images, and external representations.



/// \defgroup group-img-io Image I/O (General)
/// Image I/O functions and classes.

/// \defgroup group-img-io-jpeg Image I/O (JPEG)
/// I/O functions and classes for JPEG images.

/// \defgroup group-img-io-png Image I/O (PNG)
/// I/O functions and classes for PNG images.

/// \defgroup group-img-io-tiff Image I/O (TIFF)
/// I/O functions and classes for TIFF images.



/// \defgroup group-img-ops Image operations
/// Functions and classes to perform operations on images and image views.
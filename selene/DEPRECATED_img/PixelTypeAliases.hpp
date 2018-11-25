// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_TYPE_ALIASES_HPP
#define SELENE_IMG_PIXEL_TYPE_ALIASES_HPP

/// @file

#include <selene/DEPRECATED_img/Pixel.hpp>

namespace sln {

using Pixel_8u1 = Pixel<std::uint8_t, 1, PixelFormat::Unknown>;  ///< 8-bit unsigned 1-channel pixel.
using Pixel_8u2 = Pixel<std::uint8_t, 2, PixelFormat::Unknown>;  ///< 8-bit unsigned 2-channel pixel.
using Pixel_8u3 = Pixel<std::uint8_t, 3, PixelFormat::Unknown>;  ///< 8-bit unsigned 3-channel pixel.
using Pixel_8u4 = Pixel<std::uint8_t, 4, PixelFormat::Unknown>;  ///< 8-bit unsigned 4-channel pixel.

using Pixel_8s1 = Pixel<std::int8_t, 1, PixelFormat::Unknown>;  ///< 8-bit signed 1-channel pixel.
using Pixel_8s2 = Pixel<std::int8_t, 2, PixelFormat::Unknown>;  ///< 8-bit signed 2-channel pixel.
using Pixel_8s3 = Pixel<std::int8_t, 3, PixelFormat::Unknown>;  ///< 8-bit signed 3-channel pixel.
using Pixel_8s4 = Pixel<std::int8_t, 4, PixelFormat::Unknown>;  ///< 8-bit signed 4-channel pixel.

using Pixel_16u1 = Pixel<std::uint16_t, 1, PixelFormat::Unknown>;  ///< 16-bit unsigned 1-channel pixel.
using Pixel_16u2 = Pixel<std::uint16_t, 2, PixelFormat::Unknown>;  ///< 16-bit unsigned 2-channel pixel.
using Pixel_16u3 = Pixel<std::uint16_t, 3, PixelFormat::Unknown>;  ///< 16-bit unsigned 3-channel pixel.
using Pixel_16u4 = Pixel<std::uint16_t, 4, PixelFormat::Unknown>;  ///< 16-bit unsigned 4-channel pixel.

using Pixel_16s1 = Pixel<std::int16_t, 1, PixelFormat::Unknown>;  ///< 16-bit signed 1-channel pixel.
using Pixel_16s2 = Pixel<std::int16_t, 2, PixelFormat::Unknown>;  ///< 16-bit signed 2-channel pixel.
using Pixel_16s3 = Pixel<std::int16_t, 3, PixelFormat::Unknown>;  ///< 16-bit signed 3-channel pixel.
using Pixel_16s4 = Pixel<std::int16_t, 4, PixelFormat::Unknown>;  ///< 16-bit signed 4-channel pixel.

using Pixel_32u1 = Pixel<std::uint32_t, 1, PixelFormat::Unknown>;  ///< 32-bit unsigned 1-channel pixel.
using Pixel_32u2 = Pixel<std::uint32_t, 2, PixelFormat::Unknown>;  ///< 32-bit unsigned 2-channel pixel.
using Pixel_32u3 = Pixel<std::uint32_t, 3, PixelFormat::Unknown>;  ///< 32-bit unsigned 3-channel pixel.
using Pixel_32u4 = Pixel<std::uint32_t, 4, PixelFormat::Unknown>;  ///< 32-bit unsigned 4-channel pixel.

using Pixel_32s1 = Pixel<std::int32_t, 1, PixelFormat::Unknown>;  ///< 32-bit signed 1-channel pixel.
using Pixel_32s2 = Pixel<std::int32_t, 2, PixelFormat::Unknown>;  ///< 32-bit signed 2-channel pixel.
using Pixel_32s3 = Pixel<std::int32_t, 3, PixelFormat::Unknown>;  ///< 32-bit signed 3-channel pixel.
using Pixel_32s4 = Pixel<std::int32_t, 4, PixelFormat::Unknown>;  ///< 32-bit signed 4-channel pixel.

using Pixel_64u1 = Pixel<std::uint64_t, 1, PixelFormat::Unknown>;  ///< 64-bit unsigned 1-channel pixel.
using Pixel_64u2 = Pixel<std::uint64_t, 2, PixelFormat::Unknown>;  ///< 64-bit unsigned 2-channel pixel.
using Pixel_64u3 = Pixel<std::uint64_t, 3, PixelFormat::Unknown>;  ///< 64-bit unsigned 3-channel pixel.
using Pixel_64u4 = Pixel<std::uint64_t, 4, PixelFormat::Unknown>;  ///< 64-bit unsigned 4-channel pixel.

using Pixel_64s1 = Pixel<std::int64_t, 1, PixelFormat::Unknown>;  ///< 64-bit signed 1-channel pixel.
using Pixel_64s2 = Pixel<std::int64_t, 2, PixelFormat::Unknown>;  ///< 64-bit signed 2-channel pixel.
using Pixel_64s3 = Pixel<std::int64_t, 3, PixelFormat::Unknown>;  ///< 64-bit signed 3-channel pixel.
using Pixel_64s4 = Pixel<std::int64_t, 4, PixelFormat::Unknown>;  ///< 64-bit signed 4-channel pixel.

using Pixel_32f1 = Pixel<float32_t, 1, PixelFormat::Unknown>;  ///< 32-bit floating point 1-channel pixel.
using Pixel_32f2 = Pixel<float32_t, 2, PixelFormat::Unknown>;  ///< 32-bit floating point 2-channel pixel.
using Pixel_32f3 = Pixel<float32_t, 3, PixelFormat::Unknown>;  ///< 32-bit floating point 3-channel pixel.
using Pixel_32f4 = Pixel<float32_t, 4, PixelFormat::Unknown>;  ///< 32-bit floating point 4-channel pixel.

using Pixel_64f1 = Pixel<float64_t, 1, PixelFormat::Unknown>;  ///< 64-bit floating point 1-channel pixel.
using Pixel_64f2 = Pixel<float64_t, 2, PixelFormat::Unknown>;  ///< 64-bit floating point 2-channel pixel.
using Pixel_64f3 = Pixel<float64_t, 3, PixelFormat::Unknown>;  ///< 64-bit floating point 3-channel pixel.
using Pixel_64f4 = Pixel<float64_t, 4, PixelFormat::Unknown>;  ///< 64-bit floating point 4-channel pixel.

#define SELENE_CREATE_PIXEL_TYPE_ALIAS(P, C) \
    template <typename T> using Pixel ## P = Pixel<T, C, PixelFormat::P>; \
    using Pixel ## P ## _8u  = Pixel<std::uint8_t,  C, PixelFormat::P>;   \
    using Pixel ## P ## _8s  = Pixel<std::int8_t,   C, PixelFormat::P>;   \
    using Pixel ## P ## _16u = Pixel<std::uint16_t, C, PixelFormat::P>;   \
    using Pixel ## P ## _16s = Pixel<std::int16_t,  C, PixelFormat::P>;   \
    using Pixel ## P ## _32u = Pixel<std::uint32_t, C, PixelFormat::P>;   \
    using Pixel ## P ## _32s = Pixel<std::int32_t,  C, PixelFormat::P>;   \
    using Pixel ## P ## _64u = Pixel<std::uint64_t, C, PixelFormat::P>;   \
    using Pixel ## P ## _64s = Pixel<std::int64_t,  C, PixelFormat::P>;   \
    using Pixel ## P ## _32f = Pixel<float32_t,     C, PixelFormat::P>;   \
    using Pixel ## P ## _64f = Pixel<float64_t,     C, PixelFormat::P>;

SELENE_CREATE_PIXEL_TYPE_ALIAS(Y, 1)
SELENE_CREATE_PIXEL_TYPE_ALIAS(YA, 2)
SELENE_CREATE_PIXEL_TYPE_ALIAS(RGB, 3)
SELENE_CREATE_PIXEL_TYPE_ALIAS(BGR, 3)
SELENE_CREATE_PIXEL_TYPE_ALIAS(YCbCr, 3)
SELENE_CREATE_PIXEL_TYPE_ALIAS(CIELab, 3)
SELENE_CREATE_PIXEL_TYPE_ALIAS(ICCLab, 3)
SELENE_CREATE_PIXEL_TYPE_ALIAS(RGBA, 4)
SELENE_CREATE_PIXEL_TYPE_ALIAS(BGRA, 4)
SELENE_CREATE_PIXEL_TYPE_ALIAS(ARGB, 4)
SELENE_CREATE_PIXEL_TYPE_ALIAS(ABGR, 4)
SELENE_CREATE_PIXEL_TYPE_ALIAS(CMYK, 4)
SELENE_CREATE_PIXEL_TYPE_ALIAS(YCCK, 4)

#undef SELENE_CREATE_PIXEL_TYPE_ALIAS

}  // namespace sln

#endif  // SELENE_IMG_PIXEL_TYPE_ALIASES_HPP


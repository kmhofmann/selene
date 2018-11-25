// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_TYPE_ALIASES_HPP
#define SELENE_IMG_IMAGE_TYPE_ALIASES_HPP

/// @file

#include <selene/old_img/Image.hpp>
#include <selene/old_img/PixelTypeAliases.hpp>

namespace sln {

using Image_8u1 = Image<Pixel<std::uint8_t, 1>>;  ///< 8-bit unsigned 1-channel image.
using Image_8u2 = Image<Pixel<std::uint8_t, 2>>;  ///< 8-bit unsigned 2-channel image.
using Image_8u3 = Image<Pixel<std::uint8_t, 3>>;  ///< 8-bit unsigned 3-channel image.
using Image_8u4 = Image<Pixel<std::uint8_t, 4>>;  ///< 8-bit unsigned 4-channel image.

using Image_8s1 = Image<Pixel<std::int8_t, 1>>;  ///< 8-bit signed 1-channel image.
using Image_8s2 = Image<Pixel<std::int8_t, 2>>;  ///< 8-bit signed 2-channel image.
using Image_8s3 = Image<Pixel<std::int8_t, 3>>;  ///< 8-bit signed 3-channel image.
using Image_8s4 = Image<Pixel<std::int8_t, 4>>;  ///< 8-bit signed 4-channel image.

using Image_16u1 = Image<Pixel<std::uint16_t, 1>>;  ///< 16-bit unsigned 1-channel image.
using Image_16u2 = Image<Pixel<std::uint16_t, 2>>;  ///< 16-bit unsigned 2-channel image.
using Image_16u3 = Image<Pixel<std::uint16_t, 3>>;  ///< 16-bit unsigned 3-channel image.
using Image_16u4 = Image<Pixel<std::uint16_t, 4>>;  ///< 16-bit unsigned 4-channel image.

using Image_16s1 = Image<Pixel<std::int16_t, 1>>;  ///< 16-bit signed 1-channel image.
using Image_16s2 = Image<Pixel<std::int16_t, 2>>;  ///< 16-bit signed 2-channel image.
using Image_16s3 = Image<Pixel<std::int16_t, 3>>;  ///< 16-bit signed 3-channel image.
using Image_16s4 = Image<Pixel<std::int16_t, 4>>;  ///< 16-bit signed 4-channel image.

using Image_32u1 = Image<Pixel<std::uint32_t, 1>>;  ///< 32-bit unsigned 1-channel image.
using Image_32u2 = Image<Pixel<std::uint32_t, 2>>;  ///< 32-bit unsigned 2-channel image.
using Image_32u3 = Image<Pixel<std::uint32_t, 3>>;  ///< 32-bit unsigned 3-channel image.
using Image_32u4 = Image<Pixel<std::uint32_t, 4>>;  ///< 32-bit unsigned 4-channel image.

using Image_32s1 = Image<Pixel<std::int32_t, 1>>;  ///< 32-bit signed 1-channel image.
using Image_32s2 = Image<Pixel<std::int32_t, 2>>;  ///< 32-bit signed 2-channel image.
using Image_32s3 = Image<Pixel<std::int32_t, 3>>;  ///< 32-bit signed 3-channel image.
using Image_32s4 = Image<Pixel<std::int32_t, 4>>;  ///< 32-bit signed 4-channel image.

using Image_64u1 = Image<Pixel<std::uint64_t, 1>>;  ///< 64-bit unsigned 1-channel image.
using Image_64u2 = Image<Pixel<std::uint64_t, 2>>;  ///< 64-bit unsigned 2-channel image.
using Image_64u3 = Image<Pixel<std::uint64_t, 3>>;  ///< 64-bit unsigned 3-channel image.
using Image_64u4 = Image<Pixel<std::uint64_t, 4>>;  ///< 64-bit unsigned 4-channel image.

using Image_64s1 = Image<Pixel<std::int64_t, 1>>;  ///< 64-bit signed 1-channel image.
using Image_64s2 = Image<Pixel<std::int64_t, 2>>;  ///< 64-bit signed 2-channel image.
using Image_64s3 = Image<Pixel<std::int64_t, 3>>;  ///< 64-bit signed 3-channel image.
using Image_64s4 = Image<Pixel<std::int64_t, 4>>;  ///< 64-bit signed 4-channel image.

using Image_32f1 = Image<Pixel<float32_t, 1>>;  ///< 32-bit floating point 1-channel image.
using Image_32f2 = Image<Pixel<float32_t, 2>>;  ///< 32-bit floating point 2-channel image.
using Image_32f3 = Image<Pixel<float32_t, 3>>;  ///< 32-bit floating point 3-channel image.
using Image_32f4 = Image<Pixel<float32_t, 4>>;  ///< 32-bit floating point 4-channel image.

using Image_64f1 = Image<Pixel<float64_t, 1>>;  ///< 64-bit floating point 1-channel image.
using Image_64f2 = Image<Pixel<float64_t, 2>>;  ///< 64-bit floating point 2-channel image.
using Image_64f3 = Image<Pixel<float64_t, 3>>;  ///< 64-bit floating point 3-channel image.
using Image_64f4 = Image<Pixel<float64_t, 4>>;  ///< 64-bit floating point 4-channel image.

#define SELENE_CREATE_IMAGE_TYPE_ALIAS(P, C) \
    template <typename T> using Image ## P = Image<Pixel<T, C, PixelFormat::P>>; \
    using Image ## P ## _8u  = Image<Pixel<std::uint8_t,  C, PixelFormat::P>>;   \
    using Image ## P ## _8s  = Image<Pixel<std::int8_t,   C, PixelFormat::P>>;   \
    using Image ## P ## _16u = Image<Pixel<std::uint16_t, C, PixelFormat::P>>;   \
    using Image ## P ## _16s = Image<Pixel<std::int16_t,  C, PixelFormat::P>>;   \
    using Image ## P ## _32u = Image<Pixel<std::uint32_t, C, PixelFormat::P>>;   \
    using Image ## P ## _32s = Image<Pixel<std::int32_t,  C, PixelFormat::P>>;   \
    using Image ## P ## _64u = Image<Pixel<std::uint64_t, C, PixelFormat::P>>;   \
    using Image ## P ## _64s = Image<Pixel<std::int64_t,  C, PixelFormat::P>>;   \
    using Image ## P ## _32f = Image<Pixel<float32_t,     C, PixelFormat::P>>;   \
    using Image ## P ## _64f = Image<Pixel<float64_t,     C, PixelFormat::P>>;

SELENE_CREATE_IMAGE_TYPE_ALIAS(Y, 1)
SELENE_CREATE_IMAGE_TYPE_ALIAS(YA, 2)
SELENE_CREATE_IMAGE_TYPE_ALIAS(RGB, 3)
SELENE_CREATE_IMAGE_TYPE_ALIAS(BGR, 3)
SELENE_CREATE_IMAGE_TYPE_ALIAS(YCbCr, 3)
SELENE_CREATE_IMAGE_TYPE_ALIAS(CIELab, 3)
SELENE_CREATE_IMAGE_TYPE_ALIAS(ICCLab, 3)
SELENE_CREATE_IMAGE_TYPE_ALIAS(RGBA, 4)
SELENE_CREATE_IMAGE_TYPE_ALIAS(BGRA, 4)
SELENE_CREATE_IMAGE_TYPE_ALIAS(ARGB, 4)
SELENE_CREATE_IMAGE_TYPE_ALIAS(ABGR, 4)
SELENE_CREATE_IMAGE_TYPE_ALIAS(CMYK, 4)
SELENE_CREATE_IMAGE_TYPE_ALIAS(YCCK, 4)

#undef SELENE_CREATE_IMAGE_TYPE_ALIAS

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_TYPE_ALIASES_HPP



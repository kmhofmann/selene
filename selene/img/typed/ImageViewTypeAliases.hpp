// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_VIEW_TYPE_ALIASES_HPP
#define SELENE_IMG_IMAGE_VIEW_TYPE_ALIASES_HPP

/// @file

#include <selene/img/typed/ImageView.hpp>
#include <selene/img/pixel/Pixel.hpp>

namespace sln {

using ImageView_8u1 = ImageView<Pixel<std::uint8_t, 1>>;  ///< 8-bit unsigned 1-channel image view.
using ImageView_8u2 = ImageView<Pixel<std::uint8_t, 2>>;  ///< 8-bit unsigned 2-channel image view.
using ImageView_8u3 = ImageView<Pixel<std::uint8_t, 3>>;  ///< 8-bit unsigned 3-channel image view.
using ImageView_8u4 = ImageView<Pixel<std::uint8_t, 4>>;  ///< 8-bit unsigned 4-channel image view.

using ImageView_8s1 = ImageView<Pixel<std::int8_t, 1>>;  ///< 8-bit signed 1-channel image view.
using ImageView_8s2 = ImageView<Pixel<std::int8_t, 2>>;  ///< 8-bit signed 2-channel image view.
using ImageView_8s3 = ImageView<Pixel<std::int8_t, 3>>;  ///< 8-bit signed 3-channel image view.
using ImageView_8s4 = ImageView<Pixel<std::int8_t, 4>>;  ///< 8-bit signed 4-channel image view.

using ImageView_16u1 = ImageView<Pixel<std::uint16_t, 1>>;  ///< 16-bit unsigned 1-channel image view.
using ImageView_16u2 = ImageView<Pixel<std::uint16_t, 2>>;  ///< 16-bit unsigned 2-channel image view.
using ImageView_16u3 = ImageView<Pixel<std::uint16_t, 3>>;  ///< 16-bit unsigned 3-channel image view.
using ImageView_16u4 = ImageView<Pixel<std::uint16_t, 4>>;  ///< 16-bit unsigned 4-channel image view.

using ImageView_16s1 = ImageView<Pixel<std::int16_t, 1>>;  ///< 16-bit signed 1-channel image view.
using ImageView_16s2 = ImageView<Pixel<std::int16_t, 2>>;  ///< 16-bit signed 2-channel image view.
using ImageView_16s3 = ImageView<Pixel<std::int16_t, 3>>;  ///< 16-bit signed 3-channel image view.
using ImageView_16s4 = ImageView<Pixel<std::int16_t, 4>>;  ///< 16-bit signed 4-channel image view.

using ImageView_32u1 = ImageView<Pixel<std::uint32_t, 1>>;  ///< 32-bit unsigned 1-channel image view.
using ImageView_32u2 = ImageView<Pixel<std::uint32_t, 2>>;  ///< 32-bit unsigned 2-channel image view.
using ImageView_32u3 = ImageView<Pixel<std::uint32_t, 3>>;  ///< 32-bit unsigned 3-channel image view.
using ImageView_32u4 = ImageView<Pixel<std::uint32_t, 4>>;  ///< 32-bit unsigned 4-channel image view.

using ImageView_32s1 = ImageView<Pixel<std::int32_t, 1>>;  ///< 32-bit signed 1-channel image view.
using ImageView_32s2 = ImageView<Pixel<std::int32_t, 2>>;  ///< 32-bit signed 2-channel image view.
using ImageView_32s3 = ImageView<Pixel<std::int32_t, 3>>;  ///< 32-bit signed 3-channel image view.
using ImageView_32s4 = ImageView<Pixel<std::int32_t, 4>>;  ///< 32-bit signed 4-channel image view.

using ImageView_64u1 = ImageView<Pixel<std::uint64_t, 1>>;  ///< 64-bit unsigned 1-channel image view.
using ImageView_64u2 = ImageView<Pixel<std::uint64_t, 2>>;  ///< 64-bit unsigned 2-channel image view.
using ImageView_64u3 = ImageView<Pixel<std::uint64_t, 3>>;  ///< 64-bit unsigned 3-channel image view.
using ImageView_64u4 = ImageView<Pixel<std::uint64_t, 4>>;  ///< 64-bit unsigned 4-channel image view.

using ImageView_64s1 = ImageView<Pixel<std::int64_t, 1>>;  ///< 64-bit signed 1-channel image view.
using ImageView_64s2 = ImageView<Pixel<std::int64_t, 2>>;  ///< 64-bit signed 2-channel image view.
using ImageView_64s3 = ImageView<Pixel<std::int64_t, 3>>;  ///< 64-bit signed 3-channel image view.
using ImageView_64s4 = ImageView<Pixel<std::int64_t, 4>>;  ///< 64-bit signed 4-channel image view.

using ImageView_32f1 = ImageView<Pixel<float32_t, 1>>;  ///< 32-bit floating point 1-channel image view.
using ImageView_32f2 = ImageView<Pixel<float32_t, 2>>;  ///< 32-bit floating point 2-channel image view.
using ImageView_32f3 = ImageView<Pixel<float32_t, 3>>;  ///< 32-bit floating point 3-channel image view.
using ImageView_32f4 = ImageView<Pixel<float32_t, 4>>;  ///< 32-bit floating point 4-channel image view.

using ImageView_64f1 = ImageView<Pixel<float64_t, 1>>;  ///< 64-bit floating point 1-channel image view.
using ImageView_64f2 = ImageView<Pixel<float64_t, 2>>;  ///< 64-bit floating point 2-channel image view.
using ImageView_64f3 = ImageView<Pixel<float64_t, 3>>;  ///< 64-bit floating point 3-channel image view.
using ImageView_64f4 = ImageView<Pixel<float64_t, 4>>;  ///< 64-bit floating point 4-channel image view.

#define SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(P, C) \
    template <typename T> using ImageView ## P = ImageView<Pixel<T, C, PixelFormat::P>>; \
    using ImageView ## P ## _8u  = ImageView<Pixel<std::uint8_t,  C, PixelFormat::P>>;   \
    using ImageView ## P ## _8s  = ImageView<Pixel<std::int8_t,   C, PixelFormat::P>>;   \
    using ImageView ## P ## _16u = ImageView<Pixel<std::uint16_t, C, PixelFormat::P>>;   \
    using ImageView ## P ## _16s = ImageView<Pixel<std::int16_t,  C, PixelFormat::P>>;   \
    using ImageView ## P ## _32u = ImageView<Pixel<std::uint32_t, C, PixelFormat::P>>;   \
    using ImageView ## P ## _32s = ImageView<Pixel<std::int32_t,  C, PixelFormat::P>>;   \
    using ImageView ## P ## _64u = ImageView<Pixel<std::uint64_t, C, PixelFormat::P>>;   \
    using ImageView ## P ## _64s = ImageView<Pixel<std::int64_t,  C, PixelFormat::P>>;   \
    using ImageView ## P ## _32f = ImageView<Pixel<float32_t,     C, PixelFormat::P>>;   \
    using ImageView ## P ## _64f = ImageView<Pixel<float64_t,     C, PixelFormat::P>>;

SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(Y, 1)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(YA, 2)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(RGB, 3)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(BGR, 3)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(YCbCr, 3)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(CIELab, 3)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(ICCLab, 3)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(RGBA, 4)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(BGRA, 4)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(ARGB, 4)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(ABGR, 4)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(CMYK, 4)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(YCCK, 4)

#undef SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_VIEW_TYPE_ALIASES_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_VIEW_TYPE_ALIASES_HPP
#define SELENE_IMG_TYPED_IMAGE_VIEW_TYPE_ALIASES_HPP

/// @file

#include <selene/img/typed/ImageView.hpp>
#include <selene/img/pixel/Pixel.hpp>

namespace sln {

/// \addtogroup group-img-typed-alias
/// @{

// clang-format off

#define SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(TMPL, MOD_PRE, C, MOD) \
    TMPL using MOD_PRE ## ImageView_8u  ## C = ImageView<Pixel<std::uint8_t,  C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_8s  ## C = ImageView<Pixel<std::int8_t,   C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_16u ## C = ImageView<Pixel<std::uint16_t, C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_16s ## C = ImageView<Pixel<std::int16_t,  C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_32u ## C = ImageView<Pixel<std::uint32_t, C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_32s ## C = ImageView<Pixel<std::int32_t,  C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_64u ## C = ImageView<Pixel<std::uint64_t, C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_64s ## C = ImageView<Pixel<std::int64_t,  C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_32f ## C = ImageView<Pixel<float32_t,     C>, MOD>; \
    TMPL using MOD_PRE ## ImageView_64f ## C = ImageView<Pixel<float64_t,     C>, MOD>; \

SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(template <ImageModifiability modifiability>, , 1, modifiability)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(template <ImageModifiability modifiability>, , 2, modifiability)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(template <ImageModifiability modifiability>, , 3, modifiability)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(template <ImageModifiability modifiability>, , 4, modifiability)

SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Mutable, 1, ImageModifiability::Mutable)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Mutable, 2, ImageModifiability::Mutable)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Mutable, 3, ImageModifiability::Mutable)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Mutable, 4, ImageModifiability::Mutable)

SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Constant, 1, ImageModifiability::Constant)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Constant, 2, ImageModifiability::Constant)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Constant, 3, ImageModifiability::Constant)
SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(, Constant, 4, ImageModifiability::Constant)

#undef SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS

#define SELENE_CREATE_IMAGE_VIEW_TYPE_ALIAS(P, C) \
    template <typename T> using MutableImageView ## P = ImageView<Pixel<T, C, PixelFormat::P>, ImageModifiability::Mutable>;   \
    using MutableImageView ## P ## _8u  = ImageView<Pixel<std::uint8_t,  C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _8s  = ImageView<Pixel<std::int8_t,   C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _16u = ImageView<Pixel<std::uint16_t, C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _16s = ImageView<Pixel<std::int16_t,  C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _32u = ImageView<Pixel<std::uint32_t, C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _32s = ImageView<Pixel<std::int32_t,  C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _64u = ImageView<Pixel<std::uint64_t, C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _64s = ImageView<Pixel<std::int64_t,  C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _32f = ImageView<Pixel<float32_t,     C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    using MutableImageView ## P ## _64f = ImageView<Pixel<float64_t,     C, PixelFormat::P>, ImageModifiability::Mutable>;     \
    template <typename T> using ConstantImageView ## P = ImageView<Pixel<T, C, PixelFormat::P>, ImageModifiability::Constant>; \
    using ConstantImageView ## P ## _8u  = ImageView<Pixel<std::uint8_t,  C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _8s  = ImageView<Pixel<std::int8_t,   C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _16u = ImageView<Pixel<std::uint16_t, C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _16s = ImageView<Pixel<std::int16_t,  C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _32u = ImageView<Pixel<std::uint32_t, C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _32s = ImageView<Pixel<std::int32_t,  C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _64u = ImageView<Pixel<std::uint64_t, C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _64s = ImageView<Pixel<std::int64_t,  C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _32f = ImageView<Pixel<float32_t,     C, PixelFormat::P>, ImageModifiability::Constant>;   \
    using ConstantImageView ## P ## _64f = ImageView<Pixel<float64_t,     C, PixelFormat::P>, ImageModifiability::Constant>;   \

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

// clang-format on

/// @}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_VIEW_TYPE_ALIASES_HPP

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DETAIL_JPEG_COMMON_HPP
#define SELENE_IMG_DETAIL_JPEG_COMMON_HPP

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/MessageLog.hpp>

#include <selene/img/PixelFormat.hpp>

namespace selene {
namespace img {

/// \cond INTERNAL

enum class JPEGColorSpace : std::uint8_t
{
  Unknown,
  Grayscale,
  RGB,
  YCbCr,
  CMYK,
  YCCK,
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
  BGR,
  RGBA,
  BGRA,
  ABGR,
  ARGB,
#endif
  Auto
};

inline JPEGColorSpace pixel_format_to_color_space(PixelFormat pixel_format)
{
  switch (pixel_format)
  {
    case PixelFormat::Y: return JPEGColorSpace::Grayscale;
    case PixelFormat::RGB: return JPEGColorSpace::RGB;
    case PixelFormat::YCbCr: return JPEGColorSpace::YCbCr;
    case PixelFormat::CMYK: return JPEGColorSpace::CMYK;
    case PixelFormat::YCCK: return JPEGColorSpace::YCCK;
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    case PixelFormat::BGR: return JPEGColorSpace::BGR;
    case PixelFormat::RGBA: return JPEGColorSpace::RGBA;
    case PixelFormat::BGRA: return JPEGColorSpace::BGRA;
    case PixelFormat::ARGB: return JPEGColorSpace::ARGB;
    case PixelFormat::ABGR: return JPEGColorSpace::ABGR;
#endif
    default: return JPEGColorSpace::Unknown;
  }
}

inline PixelFormat color_space_to_pixel_format(JPEGColorSpace color_space)
{
  switch (color_space)
  {
    case JPEGColorSpace::Grayscale: return PixelFormat::Y;
    case JPEGColorSpace::RGB: return PixelFormat::RGB;
    case JPEGColorSpace::YCbCr: return PixelFormat::YCbCr;
    case JPEGColorSpace::CMYK: return PixelFormat::CMYK;
    case JPEGColorSpace::YCCK: return PixelFormat::YCCK;
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    case JPEGColorSpace::BGR: return PixelFormat::BGR;
    case JPEGColorSpace::RGBA: return PixelFormat::RGBA;
    case JPEGColorSpace::BGRA: return PixelFormat::BGRA;
    case JPEGColorSpace::ARGB: return PixelFormat::ARGB;
    case JPEGColorSpace::ABGR: return PixelFormat::ABGR;
#endif
    default: return PixelFormat::Unknown;
  }
}

/// \endcond

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBJPEG)

#endif // SELENE_IMG_DETAIL_JPEG_COMMON_HPP
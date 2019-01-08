// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_JPEG_COMMON_HPP
#define SELENE_IMG_IMPL_JPEG_COMMON_HPP

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/MessageLog.hpp>

#include <selene/img/common/PixelFormat.hpp>
#include <selene/img_io/JPEGCommon.hpp>

namespace sln {
namespace impl {

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
    case PixelFormat::BGR: return JPEGColorSpace::EXT_BGR;
    case PixelFormat::RGBA: return JPEGColorSpace::EXT_RGBA;
    case PixelFormat::BGRA: return JPEGColorSpace::EXT_BGRA;
    case PixelFormat::ARGB: return JPEGColorSpace::EXT_ARGB;
    case PixelFormat::ABGR: return JPEGColorSpace::EXT_ABGR;
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
    case JPEGColorSpace::EXT_RGB: return PixelFormat::RGB;
    case JPEGColorSpace::EXT_BGR: return PixelFormat::BGR;
    case JPEGColorSpace::EXT_RGBA: return PixelFormat::RGBA;
    case JPEGColorSpace::EXT_BGRA: return PixelFormat::BGRA;
    case JPEGColorSpace::EXT_ARGB: return PixelFormat::ARGB;
    case JPEGColorSpace::EXT_ABGR: return PixelFormat::ABGR;
    case JPEGColorSpace::EXT_RGBX: return PixelFormat::RGBA;  // interpreting X as A here...
    case JPEGColorSpace::EXT_BGRX: return PixelFormat::BGRA;  // interpreting X as A here...
    case JPEGColorSpace::EXT_XRGB: return PixelFormat::ARGB;  // interpreting X as A here...
    case JPEGColorSpace::EXT_XBGR: return PixelFormat::ABGR;  // interpreting X as A here...
#endif
    default: return PixelFormat::Unknown;
  }
}

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_IMPL_JPEG_COMMON_HPP

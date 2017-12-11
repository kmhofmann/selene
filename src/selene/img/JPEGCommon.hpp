// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_COMMON_HPP
#define SELENE_IMG_JPEG_COMMON_HPP

#if defined(SELENE_WITH_LIBJPEG)

#include <cstdint>

namespace sln {

/** \brief The color space used JPEG image data.
 */
enum class JPEGColorSpace : std::uint8_t
{
  Unknown,  ///< Unknown.
  Grayscale,  ///< 1-channel grayscale
  RGB,  ///< 3-channel RGB
  YCbCr,  ///< 3-channel YCbCr
  CMYK,  ///< 4-channel CMYK
  YCCK,  ///< 4-channel YCCK
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
  EXT_RGB,  ///< 3-channel RGB (libjpeg-turbo)
  EXT_BGR,  ///< 3-channel BGR (libjpeg-turbo)
  EXT_RGBX,  ///< 4-channel RGBX (libjpeg-turbo)
  EXT_BGRX,  ///< 4-channel BGRX (libjpeg-turbo)
  EXT_XBGR,  ///< 4-channel XBGR (libjpeg-turbo)
  EXT_XRGB,  ///< 4-channel XRGB (libjpeg-turbo)
  EXT_RGBA,  ///< 4-channel RGBA (libjpeg-turbo)
  EXT_BGRA,  ///< 4-channel BGRA (libjpeg-turbo)
  EXT_ABGR,  ///< 4-channel ABGR (libjpeg-turbo)
  EXT_ARGB,  ///< 4-channel ARGB (libjpeg-turbo)
#endif
  Auto  ///< Automatic determination
};

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_JPEG_COMMON_HPP

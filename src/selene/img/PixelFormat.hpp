// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_FORMAT_HPP
#define SELENE_IMG_PIXEL_FORMAT_HPP

#include <cstdlib>

namespace selene {
namespace img {

enum class PixelFormat : unsigned char
{
  Y,      // 1-channel: Grayscale
  X,      // 1-channel: Unknown

  YA,     // 2-channel: Grayscale & Transparency
  XX,     // 2-channel: Unknown

  RGB,    // 3-channel: RGB
  BGR,    // 3-channel: BGR
  YCbCr,  // 3-channel: YCbCr
  CIELab, // 3-channel: CIELab
  ICCLab, // 3-channel: ICCLab
  XXX,    // 3-channel: Unknown

  RGBA,   // 4-channel: RGBA
  BGRA,   // 4-channel: BGRA
  ARGB,   // 4-channel: ARGB
  ABGR,   // 4-channel: ABGR
  CMYK,   // 4-channel: CMYK
  YCCK,   // 4-channel: YCCK
  XXXX,   // 4-channel: Unknown

  Unknown // Completely unknown
};

enum class SampleType : unsigned char
{
  UnsignedInteger,
  SignedInteger,
  FloatingPoint,
  Unknown
};

inline std::size_t get_nr_channels(PixelFormat pixel_format)
{
  switch (pixel_format)
  {
    case PixelFormat::Y: return 1;
    case PixelFormat::X: return 1;

    case PixelFormat::YA: return 2;
    case PixelFormat::XX: return 2;

    case PixelFormat::RGB: return 3;
    case PixelFormat::BGR: return 3;
    case PixelFormat::YCbCr: return 3;
    case PixelFormat::CIELab: return 3;
    case PixelFormat::ICCLab: return 3;
    case PixelFormat::XXX: return 3;

    case PixelFormat::RGBA: return 4;
    case PixelFormat::BGRA: return 4;
    case PixelFormat::ARGB: return 4;
    case PixelFormat::ABGR: return 4;
    case PixelFormat::CMYK: return 4;
    case PixelFormat::YCCK: return 4;
    case PixelFormat::XXXX: return 4;

    case PixelFormat::Unknown: return 0;
  }

  return 0;
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_PIXEL_FORMAT_HPP
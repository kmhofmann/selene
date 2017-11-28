// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_FORMAT_HPP
#define SELENE_IMG_PIXEL_FORMAT_HPP

/// @file

#include <cstdlib>
#include <ostream>

namespace selene {
namespace img {

/** \brief A pixel format enumeration.
 *
 * A pixel format is a semantic tag assigned to a pixel type, as part of a dynamically typed image, i.e. an `ImageData`
 * instance.
 */
enum class PixelFormat : unsigned char
{
  Y,      ///< 1-channel format: Grayscale
  X,      ///< 1-channel format: Unknown

  YA,     ///< 2-channel format: Grayscale & Transparency
  XX,     ///< 2-channel format: Unknown

  RGB,    ///< 3-channel format: RGB
  BGR,    ///< 3-channel format: BGR
  YCbCr,  ///< 3-channel format: YCbCr
  CIELab, ///< 3-channel format: CIELab
  ICCLab, ///< 3-channel format: ICCLab
  XXX,    ///< 3-channel format: Unknown

  RGBA,   ///< 4-channel format: RGBA
  BGRA,   ///< 4-channel format: BGRA
  ARGB,   ///< 4-channel format: ARGB
  ABGR,   ///< 4-channel format: ABGR
  CMYK,   ///< 4-channel format: CMYK
  YCCK,   ///< 4-channel format: YCCK
  XXXX,   ///< 4-channel format: Unknown

  Unknown ///< Completely unknown pixel format.
};

/** \brief A sample type enumeration.
 *
 * The sample type is a semantic tag assigned to a pixel sample type (i.e. the per-channel value type of a pixel), as
 * part of a dynamically typed image, i.e. an `ImageData` instance.
 */
enum class SampleType : unsigned char
{
  UnsignedInteger,
  SignedInteger,
  FloatingPoint,
  Unknown
};

std::size_t get_nr_channels(PixelFormat pixel_format);

std::ostream& operator <<(std::ostream& os, PixelFormat pixel_format);

std::ostream& operator <<(std::ostream& os, SampleType sample_type);

// ----------
// Implementation:

/** \brief Returns the number of channels for a given pixel format.
 *
 * For example, the RGB pixel format has 3 channels; etc.
 *
 * @param pixel_format A pixel format value.
 * @return The number of channels of the provided pixel format.
 */
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
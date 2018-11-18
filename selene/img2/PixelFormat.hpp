// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_PIXEL_FORMAT_HPP
#define SELENE_IMG2_PIXEL_FORMAT_HPP

/// @file

#include <cstdlib>
#include <ostream>

namespace sln {

/** \brief A pixel format enumeration.
 *
 * A pixel format is a semantic tag assigned to a pixel type. For example, as part of a dynamically typed image, i.e. an
 * `ImageData` instance.
 */
enum class PixelFormat : unsigned char
{
  Y,  ///< 1-channel format: Grayscale
  X,  ///< 1-channel format: Unknown

  YA,  ///< 2-channel format: Grayscale & Transparency
  XX,  ///< 2-channel format: Unknown

  RGB,  ///< 3-channel format: RGB
  BGR,  ///< 3-channel format: BGR
  YCbCr,  ///< 3-channel format: YCbCr
  CIELab,  ///< 3-channel format: CIELab
  ICCLab,  ///< 3-channel format: ICCLab
  XXX,  ///< 3-channel format: Unknown

  RGBA,  ///< 4-channel format: RGBA
  BGRA,  ///< 4-channel format: BGRA
  ARGB,  ///< 4-channel format: ARGB
  ABGR,  ///< 4-channel format: ABGR
  CMYK,  ///< 4-channel format: CMYK
  YCCK,  ///< 4-channel format: YCCK
  XXXX,  ///< 4-channel format: Unknown

  Unknown,  ///< Completely unknown pixel format.
  Invalid,  ///< Invalid pixel format.
};

/** \brief A sample format enumeration.
 *
 * The sample format is a semantic tag assigned to a pixel sample type (i.e. the per-channel value type of a pixel), as
 * part of a dynamically typed image, i.e. an `ImageData` instance.
 */
enum class SampleFormat : unsigned char
{
  UnsignedInteger,
  SignedInteger,
  FloatingPoint,
  Unknown
};

constexpr std::size_t get_nr_channels(PixelFormat pixel_format) noexcept;
constexpr bool has_alpha_channel(PixelFormat pixel_format) noexcept;
constexpr bool conversion_requires_alpha_value(PixelFormat pixel_format_src, PixelFormat pixel_format_dst) noexcept;

std::ostream& operator<<(std::ostream& os, PixelFormat pixel_format);

std::ostream& operator<<(std::ostream& os, SampleFormat sample_format);

// ----------
// Implementation:

/** \brief Returns the number of channels for a given pixel format.
 *
 * For example, the RGB pixel format has 3 channels; etc.
 *
 * @param pixel_format A pixel format value.
 * @return The number of channels of the provided pixel format.
 */
inline constexpr std::size_t get_nr_channels(PixelFormat pixel_format) noexcept
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
    case PixelFormat::Invalid: return 0;
  }

  return 0;
}

/** \brief Returns whether the given pixel format has an alpha channel.
 *
 * @param pixel_format A pixel format value.
 * @return True, if the pixel format has an alpha channel; false otherwise.
 */
inline constexpr bool has_alpha_channel(PixelFormat pixel_format) noexcept
{
  switch (pixel_format)
  {
    case PixelFormat::Y: return false;
    case PixelFormat::X: return false;

    case PixelFormat::YA: return true;
    case PixelFormat::XX: return false;

    case PixelFormat::RGB: return false;
    case PixelFormat::BGR: return false;
    case PixelFormat::YCbCr: return false;
    case PixelFormat::CIELab: return false;
    case PixelFormat::ICCLab: return false;
    case PixelFormat::XXX: return false;

    case PixelFormat::RGBA: return true;
    case PixelFormat::BGRA: return true;
    case PixelFormat::ARGB: return true;
    case PixelFormat::ABGR: return true;
    case PixelFormat::CMYK: return false;
    case PixelFormat::YCCK: return false;
    case PixelFormat::XXXX: return false;

    case PixelFormat::Unknown: return false;
    case PixelFormat::Invalid: return false;
  }

  return false;
}

/** \brief Returns whether, for conversion of a pixel value from a source to a target pixel format, an additional
 * alpha value has to be specified.
 *
 * @param pixel_format_src The source pixel format.
 * @param pixel_format_dst The target pixel format.
 * @return True, if conversion of a pixel between the format requires additional specification of an alpha value; false
 * otherwise.
 */
inline constexpr bool conversion_requires_alpha_value(PixelFormat pixel_format_src,
                                                      PixelFormat pixel_format_dst) noexcept
{
  return !has_alpha_channel(pixel_format_src) && has_alpha_channel(pixel_format_dst);
}

}  // namespace sln

#endif  // SELENE_IMG2_PIXEL_FORMAT_HPP

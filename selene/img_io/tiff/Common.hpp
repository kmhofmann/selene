// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_TIFF_COMMON_HPP
#define SELENE_IMG_IO_TIFF_COMMON_HPP

/// @file

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <selene/img/common/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <ostream>

namespace sln {

/// \addtogroup group-img-io-tiff
/// @{

/** \brief Configuration of multi-channel data storage inside a TIFF file (or the current TIFF "directory").
 *
 * Multi-channel data can be stored contiguously (interleaved), or separate (planar).
 */
enum class TIFFPlanarConfig
{
  Unknown,  ///< Storage is unknown.
  Contiguous,  ///< Storage is contiguous (interleaved).
  Separate,  ///< Storate is separate (planar).
};

/** \brief The photometric tag assigned in the TIFF file (or the current TIFF "directory").
 */
enum class TIFFPhotometricTag
{
  Unknown,
  MinIsWhite,
  MinIsBlack,
  RGB,
  Palette,
  Mask,
  Separated,
  YCbCr,
  CIELab,
  ICCLab,
  ITULab,
  CFA,
  LogL,
  LogLUV,
};

/** \brief The sample format used for pixel elements in a TIFF file (or the current TIFF "directory").
 */
enum class TIFFSampleFormat
{
  Unknown,
  UnsignedInt,
  Int,
  Float,
  Void,
  ComplexInt,
  ComplexFloat,
};

/** \brief The compression scheme used in a TIFF file (or the current TIFF "directory").
 */
enum class TIFFCompression
{
  Unknown,
  None,
  CCITT_RLE,
  CCITT_Fax3,
  CCITT_Fax4,
  LZW,
  OJPEG,
  JPEG,
  NeXT,
  CCITT_RLEW,
  PackBits,
  ThunderScan,
  IT8_CT_pad,
  IT8_LineworkRLE,
  IT8MP,
  IT8BL,
  PixarLog,
  Deflate,
  JBIG,
  LZMA,
#if defined(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT)
  Zstd,  //<<< Only available with libtiff 4.0.10 or higher present.
  Webp,  //<<< Only available with libtiff 4.0.10 or higher present.
#endif
};

/** \brief Describes the layout and some storage properties of a TIFF image.
 *
 * Details about the storage type (strips or tiles), or the particular strip or tile storage details are not included
 * in this structure.
 */
struct TiffImageLayout
{
  std::uint32_t width;  ///< The image width.
  std::uint32_t height;  ///< The image height.
  std::uint32_t depth;  ///< The image depth. Selene can only read images where depth == 1.
  std::uint16_t samples_per_pixel;  ///< The number of samples (channels) per pixel.
  std::uint16_t bits_per_sample;  ///< The number of bits per sample (pixel element/channel).
  sln::TIFFPlanarConfig planar_config;  ///< The multi-channel data storage configuration (continuous or separate).
  sln::TIFFPhotometricTag photometric;  ///< The photometric tag.
  sln::TIFFSampleFormat sample_format;  ///< The sample format.
  sln::TIFFCompression compression;  ///< The data compression type.

  TiffImageLayout(std::uint32_t width_,
                  std::uint32_t height_,
                  std::uint32_t depth_,
                  std::uint16_t samples_per_pixel_,
                  std::uint16_t bits_per_sample_,
                  sln::TIFFPlanarConfig planar_config_,
                  sln::TIFFPhotometricTag photometric_,
                  sln::TIFFSampleFormat sample_format_,
                  sln::TIFFCompression compression_)
      : width(width_)
      , height(height_)
      , depth(depth_)
      , samples_per_pixel(samples_per_pixel_)
      , bits_per_sample(bits_per_sample_)
      , planar_config(planar_config_)
      , photometric(photometric_)
      , sample_format(sample_format_)
      , compression(compression_)
  {
  }

  bool inverted() const
  {
    return samples_per_pixel == 1 && photometric == TIFFPhotometricTag::MinIsWhite;
  }

  bool is_format_grayscale() const
  {
    return photometric == TIFFPhotometricTag::MinIsBlack || photometric == TIFFPhotometricTag::MinIsWhite;
  }

  bool is_format_ycbcr() const
  {
    return photometric == TIFFPhotometricTag::YCbCr;
  }

  bool is_format_lab() const
  {
    return photometric == TIFFPhotometricTag::CIELab
        || photometric == TIFFPhotometricTag::ICCLab
        || photometric == TIFFPhotometricTag::ITULab;
  }

  bool is_format_palette() const
  {
    return photometric == TIFFPhotometricTag::Palette;
  }

  PixelLength width_px() const
  {
    return to_pixel_length(width);
  }

  PixelLength height_px() const
  {
    return to_pixel_length(height);
  }

  std::int16_t nr_channels() const
  {
    return static_cast<std::int16_t>(samples_per_pixel);
  }

  std::int16_t nr_bytes_per_channel() const
  {
    return std::min(std::int16_t{1}, static_cast<std::int16_t>(bits_per_sample >> 3));
  }
};

std::ostream& operator<<(std::ostream& os, const TiffImageLayout& info);

MessageLog global_tiff_message_log();

void clear_global_tiff_message_log();

namespace impl {

void tiff_set_handlers();
void tiff_assign_message_log(const MessageLog& message_log, MessageLog* output_message_log);

}  // namespace impl

/// @}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_COMMON_HPP
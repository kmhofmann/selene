// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_TIFF_COMMON_HPP
#define SELENE_IMG_IO_TIFF_COMMON_HPP

/// @file

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <cstdint>
#include <ostream>

namespace sln {

enum class TIFFPlanarConfig
{
  Unknown,
  Contiguous,
  Separate,
};

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
  Zstd,
  Webp,
#endif
};

struct TiffImageLayout
{
  std::uint32_t width;
  std::uint32_t height;
  std::uint32_t depth;
  std::uint16_t samples_per_pixel;
  std::uint16_t bits_per_sample;
  sln::TIFFPlanarConfig planar_config;
  sln::TIFFPhotometricTag photometric;
  sln::TIFFSampleFormat sample_format;
  sln::TIFFCompression compression;

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
};

std::ostream& operator<<(std::ostream& os, const TiffImageLayout& info);

MessageLog global_tiff_message_log();

void clear_global_tiff_message_log();

namespace impl {

void tiff_set_handlers();
void tiff_assign_message_log(const MessageLog& message_log, MessageLog* output_message_log);

}  // namespace impl

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_COMMON_HPP
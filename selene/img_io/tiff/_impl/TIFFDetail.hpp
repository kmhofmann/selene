// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_IMPL_TIFF_DETAIL_HPP
#define SELENE_IMG_IO_IMPL_TIFF_DETAIL_HPP

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>

#include <selene/img/common/PixelFormat.hpp>
#include <selene/img/common/Types.hpp>

#include <selene/img_io/tiff/Common.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <tiff.h>
#include <tiffio.h>

namespace sln {
namespace impl {
namespace tiff {

std::uint16_t planar_config_pub_to_lib(TIFFPlanarConfig cfg);
TIFFPlanarConfig planar_config_lib_to_pub(std::uint16_t cfg);

std::uint16_t photometric_tag_pub_to_lib(TIFFPhotometricTag tag);
TIFFPhotometricTag photometric_tag_lib_to_pub(std::uint16_t tag);

std::uint16_t sample_format_pub_to_lib(TIFFSampleFormat fmt);
TIFFSampleFormat sample_format_lib_to_pub(std::uint16_t fmt);

std::uint16_t compression_pub_to_lib(TIFFCompression cpr);
TIFFCompression compression_lib_to_pub(std::uint16_t cpr);

std::uint16_t orientation_pub_to_lib(TIFFOrientation o);
TIFFOrientation orientation_lib_to_pub(std::uint16_t o);

sln::PixelFormat photometric_to_pixel_format(TIFFPhotometricTag tag, std::uint16_t nr_channels);
sln::PixelFormat photometric_to_pixel_format(std::uint16_t value, std::uint16_t nr_channels);
std::uint16_t pixel_format_to_photometric(sln::PixelFormat fmt);

sln::SampleFormat sample_format_to_sample_format(TIFFSampleFormat fmt);
sln::SampleFormat sample_format_to_sample_format(std::uint16_t value);
std::uint16_t sample_format_to_sample_format(sln::SampleFormat fmt);

std::string planar_config_to_string(TIFFPlanarConfig cfg);
std::string photometric_to_string(TIFFPhotometricTag tag);
std::string sample_format_to_string(TIFFSampleFormat fmt);
std::string compression_to_string(TIFFCompression cpr);
std::string orientation_to_string(TIFFOrientation o);

std::string planar_config_to_string(std::uint16_t value);
std::string photometric_to_string(std::uint16_t value);
std::string sample_format_to_string(std::uint16_t value);
std::string compression_to_string(std::uint16_t value);
std::string orientation_to_string(std::uint16_t value);

template <typename T> T get_field(TIFF* tif, uint32 tag)
{
  T var{};
  const int val = TIFFGetFieldDefaulted(tif, tag, &var);
  if (val == 0)
  {
    throw std::runtime_error("Error getting field");
  }
  return var;
}

template <typename T> T get_field(TIFF* tif, uint32 tag, T default_value)
{
  T var{};
  const auto val = TIFFGetFieldDefaulted(tif, tag, &var);
  return (val == 1) ? var : default_value;
}

template <typename T> std::pair<T, T> get_field_2(TIFF* tif, uint32 tag)
{
  T var0{}, var1{};
  const int val = TIFFGetFieldDefaulted(tif, tag, &var0, &var1);
  if (val == 0)
  {
    throw std::runtime_error("Error getting field");
  }
  return std::make_pair(var0, var1);
}

template <typename T> std::pair<T, T> get_field_2(TIFF* tif, uint32 tag, T default_value)
{
  T var0{}, var1{};
  const int val = TIFFGetFieldDefaulted(tif, tag, &var0, &var1);
  return (val == 1) ? std::make_pair(var0, var1) : std::make_pair(default_value, default_value);
}

inline std::string get_string_field(TIFF* tif, uint32 tag)
{
  char* buf = nullptr;
  const int val = TIFFGetFieldDefaulted(tif, tag, &buf);
  if (val == 0 || buf == nullptr)
  {
    return std::string{};
  }
  return std::string{buf};
}

template <typename... Ts>
inline void set_field(TIFF* tif, uint32 tag, Ts... values)
{
  [[maybe_unused]] int res = TIFFSetField(tif, tag, values...);
  SELENE_ASSERT(res != 0);
}

inline void set_string_field(TIFF* tif, uint32 tag, const std::string& str)
{
  [[maybe_unused]] int res = TIFFSetField(tif, tag, str.c_str());
  SELENE_ASSERT(res != 0);
}

struct ImageLayoutStrips
{
  uint32 nr_strips;
  tmsize_t size_bytes;
  uint32 rows_per_strip;

  ImageLayoutStrips(uint32 nr_strips_, tmsize_t strip_size_, uint32 rows_per_strip_)
      : nr_strips(nr_strips_), size_bytes(strip_size_), rows_per_strip(rows_per_strip_)
  {
  }
};

struct ImageLayoutTiles
{
  uint32 width;
  uint32 height;
  uint32 depth;

  tmsize_t size_bytes;

  ImageLayoutTiles(uint32 tile_width, uint32 tile_height, uint32 tile_depth, tmsize_t tile_size)
      : width(tile_width), height(tile_height), depth(tile_depth), size_bytes(tile_size)
  {
    SELENE_ASSERT(depth == 1);
  }
};

struct YCbCrInfo
{
  float coeff_red;
  float coeff_green;
  float coeff_blue;
  uint16 positioning;
  uint16 subsampling_horz;
  uint16 subsampling_vert;

  YCbCrInfo(float coeff_red_, float coeff_green_, float coeff_blue_, uint16 positioning_,
                uint16 subsampling_horz_, uint16 subsampling_vert_)
      : coeff_red(coeff_red_), coeff_green(coeff_green_), coeff_blue(coeff_blue_), positioning(positioning_),
        subsampling_horz(subsampling_horz_), subsampling_vert(subsampling_vert_)
  {
  }

  bool check_strip_size(uint32 width, uint32 height, uint32 rows_per_strip, MessageLog& message_log) const
  {
    bool subsample_pars_check = check_subsampling_parameters(message_log);
    bool size_pars_check = check_size(width, height, message_log);

    if (!subsample_pars_check || !size_pars_check)
    {
      return false;
    }

    if (rows_per_strip % subsampling_vert != 0)
    {
      message_log.add("WARNING: Invalid rows_per_strip for YCbCr y-subsampling parameter", MessageType::Warning);
      return false;
    }

    return true;
  }

  bool check_tile_size(uint32 width, uint32 height, uint32 tile_width, uint32 tile_height, MessageLog& message_log) const
  {
    bool subsample_pars_check = check_subsampling_parameters(message_log);
    bool size_pars_check = check_size(width, height, message_log);

    if (!subsample_pars_check || !size_pars_check)
    {
      return false;
    }

    if (tile_width % subsampling_horz != 0 || tile_height % subsampling_vert != 0)
    {
      message_log.add("WARNING: Invalid tile_width/tile_height for YCbCr subsampling parameters", MessageType::Warning);
      return false;
    }

    return true;
  }

private:
  [[nodiscard]] bool check_subsampling_parameters(MessageLog& message_log) const
  {
    // See also https://www.awaresystems.be/imaging/tiff/tifftags/ycbcrsubsampling.html
    if (subsampling_horz != 1 && subsampling_horz != 2 && subsampling_horz != 4)
    {
      message_log.add("WARNING: Invalid YCbCr subsampling parameters", MessageType::Warning);
      return false;
    }

    if (subsampling_vert != 1 && subsampling_vert != 2 && subsampling_vert != 4)
    {
      message_log.add("WARNING: Invalid YCbCr subsampling parameters", MessageType::Warning);
      return false;
    }

    if (subsampling_vert > subsampling_horz)
    {
      message_log.add("WARNING: Invalid YCbCr subsampling parameters", MessageType::Warning);
      return false;
    }

    return true;
  }

  [[nodiscard]] bool check_size(uint32 width, uint32 height, MessageLog& message_log) const
  {
    if (width % subsampling_horz != 0 || height % subsampling_vert != 0)
    {
      message_log.add("WARNING: Invalid width/height for YCbCr subsampling parameters", MessageType::Warning);
      return false;
    }

    return true;
  }
};

class YCbCrConverter
{
public:
  YCbCrConverter(float* ycbcr_coefficients, float* reference_blackwhite)
  {
    ycbcr_ = (TIFFYCbCrToRGB*)_TIFFmalloc(std::max(sizeof(TIFFYCbCrToRGB), sizeof(long))
                                          + 4*256*sizeof(TIFFRGBValue) + 2*256*sizeof(int) + 3*256*sizeof(int32));
    SELENE_ASSERT(ycbcr_ != nullptr);
    if (TIFFYCbCrToRGBInit(ycbcr_, ycbcr_coefficients, reference_blackwhite) < 0)
    {
      throw std::runtime_error("Could not allocate TIFFYCbCrToRGB*");
    }
  }

  ~YCbCrConverter()
  {
    if (ycbcr_)
    {
      _TIFFfree(ycbcr_);
    }
  }

  YCbCrConverter(const YCbCrConverter&) = delete;
  YCbCrConverter& operator=(const YCbCrConverter&) = delete;

  YCbCrConverter(YCbCrConverter&& other) noexcept
      : ycbcr_(other.ycbcr_)
  {
    other.ycbcr_ = nullptr;
  }

  YCbCrConverter& operator=(YCbCrConverter&& other) noexcept
  {
    ycbcr_ = other.ycbcr_;
    other.ycbcr_ = nullptr;
    return *this;
  }

  void convert(uint32 Y, int32 Cb, int32 Cr, uint32& r, uint32& g, uint32& b) const
  {
    TIFFYCbCrtoRGB(ycbcr_, Y, Cb, Cr, &r, &g, &b);
  }

private:
  TIFFYCbCrToRGB* ycbcr_;
};

class LabConverter
{
public:
  explicit LabConverter(const float* white_point_coefficients)
  {
    cielab_ = (TIFFCIELabToRGB*)_TIFFmalloc(sizeof(TIFFCIELabToRGB));
    SELENE_ASSERT(cielab_ != nullptr);

    ref_white[1] = 100.0F;
    ref_white[0] = white_point_coefficients[0] / white_point_coefficients[1] * ref_white[1];
    ref_white[2] = (1.0F - white_point_coefficients[0] - white_point_coefficients[1]) / white_point_coefficients[1] * ref_white[1];

    if (TIFFCIELabToRGBInit(cielab_, &display_sRGB, ref_white) < 0)
    {
      throw std::runtime_error("Could not allocate TIFFCIELabToRGB*");
    }
  }

  ~LabConverter()
  {
    if (cielab_)
    {
      _TIFFfree(cielab_);
    }
  }

  LabConverter(const LabConverter&) = delete;
  LabConverter& operator=(const LabConverter&) = delete;

  LabConverter(LabConverter&& other) noexcept
      : cielab_(other.cielab_), display_sRGB(other.display_sRGB)
  {
    ref_white[0] = other.ref_white[0];
    ref_white[1] = other.ref_white[1];
    ref_white[2] = other.ref_white[2];
    other.cielab_ = nullptr;
  }

  LabConverter& operator=(LabConverter&& other) noexcept
  {
    cielab_ = other.cielab_;
    ref_white[0] = other.ref_white[0];
    ref_white[1] = other.ref_white[1];
    ref_white[2] = other.ref_white[2];
    display_sRGB = other.display_sRGB;
    other.cielab_ = nullptr;
    return *this;
  }

  void convert(uint32 lab_L, int32 lab_a, int32 lab_b, uint32& r, uint32& g, uint32& b) const
  {
    float X;
    float Y;
    float Z;
    TIFFCIELabToXYZ(cielab_, lab_L, lab_a, lab_b, &X, &Y, &Z);
    TIFFXYZToRGB(cielab_, X, Y, Z, &r, &g, &b);
  }

private:
  TIFFCIELabToRGB* cielab_;

  // http://www.simplesystems.org/libtiff/man/TIFFcolor.3tiff.html
  float ref_white[3] = {0.0f, 0.0f, 0.0f};
  TIFFDisplay display_sRGB = {
      { /* XYZ -> luminance matrix */
          { 3.2410f, -1.5374f, -0.4986f },
          { -0.9692f, 1.8760f, 0.0416f },
          { 0.0556f, -0.2040f, 1.0570f }
      },
      100.0f, 100.0f, 100.0f, /* Light o/p for reference white */
      255, 255, 255, /* Pixel values for ref. white */
      1.0f, 1.0f, 1.0f, /* Residual light o/p for black pixel */
      2.4f, 2.4f, 2.4f, /* Gamma values for the three guns */
  };
};

struct OutputLayout
{
  sln::PixelLength width;
  sln::PixelLength height;
  std::int16_t nr_channels;
  std::int16_t nr_bytes_per_channel;
  sln::PixelFormat pixel_format;
  sln::SampleFormat sample_format;

  OutputLayout(sln::PixelLength width_, sln::PixelLength height_,
               std::int16_t nr_channels_, std::int16_t nr_bytes_per_channel_,
               sln::PixelFormat pixel_format_, sln::SampleFormat sample_format_)
      : width(width_), height(height_), nr_channels(nr_channels_), nr_bytes_per_channel(nr_bytes_per_channel_),
        pixel_format(pixel_format_), sample_format(sample_format_)
  {
  }
};

std::ostream& operator<<(std::ostream& os, const ImageLayoutStrips& info);
std::ostream& operator<<(std::ostream& os, const ImageLayoutTiles& info);
std::ostream& operator<<(std::ostream& os, const YCbCrInfo& info);
std::ostream& operator<<(std::ostream& os, const OutputLayout& info);

std::uint8_t* copy_samples(const std::uint8_t* src_dense, std::size_t nr_src_pixels, std::size_t channel_offset,
                           std::int16_t nr_bytes_per_channel, std::int16_t nr_channels, std::uint8_t* dst);

std::vector<std::uint8_t> convert_single_channel_1bit_to_8bit(const std::vector<std::uint8_t>& buf,
                                                              std::ptrdiff_t nr_bytes_read,
                                                              std::uint32_t width,
                                                              std::uint32_t height);

std::vector<std::uint8_t> convert_single_channel_4bit_to_8bit(const std::vector<std::uint8_t>& buf,
                                                              std::ptrdiff_t nr_bytes_read,
                                                              std::uint32_t width,
                                                              std::uint32_t height);

std::vector<std::uint8_t> convert_ycbcr_to_rgb_interleaved(const std::vector<std::uint8_t>& buf,
                                                           std::ptrdiff_t nr_bytes_read,
                                                           std::uint32_t width,
                                                           std::uint32_t height,
                                                           const YCbCrInfo& ycbcr_info,
                                                           const YCbCrConverter& ycbcr_converter);

std::vector<std::uint8_t> convert_lab_to_rgb_interleaved(const std::vector<std::uint8_t>& buf,
                                                         std::ptrdiff_t nr_bytes_read,
                                                         std::uint32_t width,
                                                         std::uint32_t height,
                                                         const LabConverter& lab_converter);

}  // namespace tiff
}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_IMPL_TIFF_DETAIL_HPP
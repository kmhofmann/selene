// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/_impl/TIFFDetail.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageViewTypeAliases.hpp>
#include <selene/img/common/Types.hpp>

#include <mutex>
#include <stdexcept>

namespace sln {
namespace impl {
namespace tiff {

std::uint16_t planar_config_pub_to_lib(TIFFPlanarConfig cfg)
{
  switch (cfg)
  {
    case TIFFPlanarConfig::Contiguous: return PLANARCONFIG_CONTIG;
    case TIFFPlanarConfig::Separate: return PLANARCONFIG_SEPARATE;
    default: return 0;  // ?
  }
}

TIFFPlanarConfig planar_config_lib_to_pub(std::uint16_t cfg)
{
  switch (cfg)
  {
    case PLANARCONFIG_CONTIG: return TIFFPlanarConfig::Contiguous;
    case PLANARCONFIG_SEPARATE: return TIFFPlanarConfig::Separate;
    default: return TIFFPlanarConfig::Unknown;
  }
}

std::uint16_t photometric_tag_pub_to_lib(TIFFPhotometricTag tag)
{
  switch (tag)
  {
    case TIFFPhotometricTag::MinIsWhite: return PHOTOMETRIC_MINISWHITE;
    case TIFFPhotometricTag::MinIsBlack: return PHOTOMETRIC_MINISBLACK;
    case TIFFPhotometricTag::RGB: return PHOTOMETRIC_RGB;
    case TIFFPhotometricTag::Palette: return PHOTOMETRIC_PALETTE;
    case TIFFPhotometricTag::Mask: return PHOTOMETRIC_MASK;
    case TIFFPhotometricTag::Separated: return PHOTOMETRIC_SEPARATED;
    case TIFFPhotometricTag::YCbCr: return PHOTOMETRIC_YCBCR;
    case TIFFPhotometricTag::CIELab: return PHOTOMETRIC_CIELAB;
    case TIFFPhotometricTag::ICCLab: return PHOTOMETRIC_ICCLAB;
    case TIFFPhotometricTag::ITULab: return PHOTOMETRIC_ITULAB;
    case TIFFPhotometricTag::CFA: return PHOTOMETRIC_CFA;
    case TIFFPhotometricTag::LogL: return PHOTOMETRIC_LOGL;
    case TIFFPhotometricTag::LogLUV: return PHOTOMETRIC_LOGLUV;
    default: return 0;  // ?
  }
}

TIFFPhotometricTag photometric_tag_lib_to_pub(std::uint16_t tag)
{
  switch (tag)
  {
    case PHOTOMETRIC_MINISWHITE: return TIFFPhotometricTag::MinIsWhite;
    case PHOTOMETRIC_MINISBLACK: return TIFFPhotometricTag::MinIsBlack;
    case PHOTOMETRIC_RGB: return TIFFPhotometricTag::RGB;
    case PHOTOMETRIC_PALETTE: return TIFFPhotometricTag::Palette;
    case PHOTOMETRIC_MASK: return TIFFPhotometricTag::Mask;
    case PHOTOMETRIC_SEPARATED: return TIFFPhotometricTag::Separated;
    case PHOTOMETRIC_YCBCR: return TIFFPhotometricTag::YCbCr;
    case PHOTOMETRIC_CIELAB: return TIFFPhotometricTag::CIELab;
    case PHOTOMETRIC_ICCLAB: return TIFFPhotometricTag::ICCLab;
    case PHOTOMETRIC_ITULAB: return TIFFPhotometricTag::ITULab;
    case PHOTOMETRIC_CFA: return TIFFPhotometricTag::CFA;
    case PHOTOMETRIC_LOGL: return TIFFPhotometricTag::LogL;
    case PHOTOMETRIC_LOGLUV: return TIFFPhotometricTag::LogLUV;
    default: return TIFFPhotometricTag::Unknown;
  }
}

std::uint16_t sample_format_pub_to_lib(TIFFSampleFormat fmt)
{
  switch (fmt)
  {
    case TIFFSampleFormat::UnsignedInt: return SAMPLEFORMAT_UINT;
    case TIFFSampleFormat::Int: return SAMPLEFORMAT_INT;
    case TIFFSampleFormat::Float: return SAMPLEFORMAT_IEEEFP;
    case TIFFSampleFormat::Void: return SAMPLEFORMAT_VOID;
    case TIFFSampleFormat::ComplexInt: return SAMPLEFORMAT_COMPLEXINT;
    case TIFFSampleFormat::ComplexFloat: return SAMPLEFORMAT_COMPLEXIEEEFP;
    default: return 0;  // ?
  }
}

TIFFSampleFormat sample_format_lib_to_pub(std::uint16_t fmt)
{
  switch (fmt)
  {
    case SAMPLEFORMAT_UINT: return TIFFSampleFormat::UnsignedInt;
    case SAMPLEFORMAT_INT: return TIFFSampleFormat::Int;
    case SAMPLEFORMAT_IEEEFP: return TIFFSampleFormat::Float;
    case SAMPLEFORMAT_VOID: return TIFFSampleFormat::Void;
    case SAMPLEFORMAT_COMPLEXINT: return TIFFSampleFormat::ComplexInt;
    case SAMPLEFORMAT_COMPLEXIEEEFP: return TIFFSampleFormat::ComplexFloat;
    default: return TIFFSampleFormat::Unknown;
  }
}

std::uint16_t compression_pub_to_lib(TIFFCompression cpr)
{
  switch (cpr)
  {
    case TIFFCompression::None: return COMPRESSION_NONE;
    case TIFFCompression::CCITT_RLE: return COMPRESSION_CCITTRLE;
    case TIFFCompression::CCITT_Fax3: return COMPRESSION_CCITTFAX3;
    case TIFFCompression::CCITT_Fax4: return COMPRESSION_CCITTFAX4;
    case TIFFCompression::LZW: return COMPRESSION_LZW;
    case TIFFCompression::OJPEG: return COMPRESSION_OJPEG;
    case TIFFCompression::JPEG: return COMPRESSION_JPEG;
    case TIFFCompression::NeXT: return COMPRESSION_NEXT;
    case TIFFCompression::CCITT_RLEW: return COMPRESSION_CCITTRLEW;
    case TIFFCompression::PackBits: return COMPRESSION_PACKBITS;
    case TIFFCompression::ThunderScan: return COMPRESSION_THUNDERSCAN;
    case TIFFCompression::IT8_CT_pad: return COMPRESSION_IT8CTPAD;
    case TIFFCompression::IT8_LineworkRLE: return COMPRESSION_IT8LW;
    case TIFFCompression::IT8MP: return COMPRESSION_IT8MP;
    case TIFFCompression::IT8BL: return COMPRESSION_IT8BL;
    case TIFFCompression::PixarLog: return COMPRESSION_PIXARLOG;
    case TIFFCompression::Deflate: return COMPRESSION_DEFLATE;
    case TIFFCompression::JBIG: return COMPRESSION_JBIG;
    case TIFFCompression::LZMA: return COMPRESSION_LZMA;
#if defined(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT)
    case TIFFCompression::Zstd: return COMPRESSION_ZSTD;
    case TIFFCompression::Webp: return COMPRESSION_WEBP;
#endif
    default: return 0;  // ?
  }
}

TIFFCompression compression_lib_to_pub(std::uint16_t cpr)
{
  switch (cpr)
  {
    case COMPRESSION_NONE: return TIFFCompression::None;
    case COMPRESSION_CCITTRLE: return TIFFCompression::CCITT_RLE;
    case COMPRESSION_CCITTFAX3: return TIFFCompression::CCITT_Fax3;
    case COMPRESSION_CCITTFAX4: return TIFFCompression::CCITT_Fax4;
    case COMPRESSION_LZW: return TIFFCompression::LZW;
    case COMPRESSION_OJPEG: return TIFFCompression::OJPEG;
    case COMPRESSION_JPEG: return TIFFCompression::JPEG;
    case COMPRESSION_NEXT: return TIFFCompression::NeXT;
    case COMPRESSION_CCITTRLEW: return TIFFCompression::CCITT_RLEW;
    case COMPRESSION_PACKBITS: return TIFFCompression::PackBits;
    case COMPRESSION_THUNDERSCAN: return TIFFCompression::ThunderScan;
    case COMPRESSION_IT8CTPAD: return TIFFCompression::IT8_CT_pad;
    case COMPRESSION_IT8LW: return TIFFCompression::IT8_LineworkRLE;
    case COMPRESSION_IT8MP: return TIFFCompression::IT8MP;
    case COMPRESSION_IT8BL: return TIFFCompression::IT8BL;
    case COMPRESSION_PIXARLOG: return TIFFCompression::PixarLog;
    case COMPRESSION_DEFLATE: return TIFFCompression::Deflate;
    case COMPRESSION_JBIG: return TIFFCompression::JBIG;
    case COMPRESSION_LZMA: return TIFFCompression::LZMA;
#if defined(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT)
    case COMPRESSION_ZSTD: return TIFFCompression::Zstd;
    case COMPRESSION_WEBP: return TIFFCompression::Webp;
#endif
    default: return TIFFCompression::Unknown;
  }
}

// -----

sln::PixelFormat photometric_to_pixel_format(TIFFPhotometricTag tag, std::uint16_t nr_channels)
{
  return photometric_to_pixel_format(photometric_tag_pub_to_lib(tag), nr_channels);
}

sln::PixelFormat photometric_to_pixel_format(std::uint16_t value, std::uint16_t nr_channels)
{
  switch (value)
  {
    case PHOTOMETRIC_MINISWHITE: return nr_channels == 1 ? PixelFormat::Y : PixelFormat::Unknown;
    case PHOTOMETRIC_MINISBLACK: return nr_channels == 1 ? PixelFormat::Y : PixelFormat::Unknown;
    case PHOTOMETRIC_RGB: return nr_channels == 3 ? PixelFormat::RGB
                                                  : (nr_channels == 4 ? PixelFormat::RGBA : PixelFormat::Unknown);
    case PHOTOMETRIC_PALETTE: return PixelFormat::Unknown; // TODO: deal with palette images?
    case PHOTOMETRIC_MASK: return PixelFormat::Unknown; // TODO?
    case PHOTOMETRIC_SEPARATED: return nr_channels == 4 ? PixelFormat::CMYK : PixelFormat::Unknown;
    case PHOTOMETRIC_YCBCR: return nr_channels == 3 ? PixelFormat::YCbCr : PixelFormat::Unknown;
    case PHOTOMETRIC_CIELAB: return nr_channels == 3 ? PixelFormat::CIELab : PixelFormat::Unknown;
    case PHOTOMETRIC_ICCLAB: return nr_channels == 3 ? PixelFormat::ICCLab : PixelFormat::Unknown;
    case PHOTOMETRIC_ITULAB: return PixelFormat::Unknown;
    case PHOTOMETRIC_CFA: return PixelFormat::Unknown;
    case PHOTOMETRIC_LOGL: return PixelFormat::Unknown;
    case PHOTOMETRIC_LOGLUV: return PixelFormat::Unknown;
    default: return PixelFormat::Unknown;
  }
}

std::uint16_t pixel_format_to_photometric(sln::PixelFormat fmt)
{
  switch (fmt)
  {
    case PixelFormat::Y: return PHOTOMETRIC_MINISBLACK;
    case PixelFormat::RGB: return PHOTOMETRIC_RGB;
    case PixelFormat::RGBA: return PHOTOMETRIC_RGB;
    case PixelFormat::YCbCr: return PHOTOMETRIC_YCBCR;
    case PixelFormat::CIELab: return PHOTOMETRIC_CIELAB;
    case PixelFormat::ICCLab: return PHOTOMETRIC_ICCLAB;
    default: throw std::runtime_error("Cannot convert given pixel format to TIFF photometric tag.");
  }
}

sln::SampleFormat sample_format_to_sample_format(TIFFSampleFormat fmt)
{
  return sample_format_to_sample_format(sample_format_pub_to_lib(fmt));
}

sln::SampleFormat sample_format_to_sample_format(std::uint16_t value)
{
  switch (value)
  {
    case SAMPLEFORMAT_UINT: return SampleFormat::UnsignedInteger;
    case SAMPLEFORMAT_INT: return SampleFormat::SignedInteger;
    case SAMPLEFORMAT_IEEEFP: return SampleFormat::FloatingPoint;
    case SAMPLEFORMAT_VOID: return SampleFormat::Unknown;
    case SAMPLEFORMAT_COMPLEXINT: return SampleFormat::Unknown;
    case SAMPLEFORMAT_COMPLEXIEEEFP: return SampleFormat::Unknown;
    default: return SampleFormat::Unknown;
  }
}

std::uint16_t sample_format_to_sample_format(sln::SampleFormat fmt)
{
  switch (fmt)
  {
    case SampleFormat::UnsignedInteger: return SAMPLEFORMAT_UINT;
    case SampleFormat::SignedInteger: return SAMPLEFORMAT_INT;
    case SampleFormat::FloatingPoint: return SAMPLEFORMAT_IEEEFP;
    default: return SAMPLEFORMAT_VOID;  // ?
  }
}

std::string planar_config_to_string(TIFFPlanarConfig cfg)
{
  return planar_config_to_string(planar_config_pub_to_lib(cfg));
}

std::string photometric_to_string(TIFFPhotometricTag tag)
{
  return photometric_to_string(photometric_tag_pub_to_lib(tag));
}

std::string sample_format_to_string(TIFFSampleFormat fmt)
{
  return sample_format_to_string(sample_format_pub_to_lib(fmt));
}

std::string compression_to_string(TIFFCompression cpr)
{
  return compression_to_string(compression_pub_to_lib(cpr));
}

std::string planar_config_to_string(std::uint16_t value)
{
  switch (value)
  {
    case PLANARCONFIG_CONTIG: return "PLANARCONFIG_CONTIG";
    case PLANARCONFIG_SEPARATE: return "PLANARCONFIG_SEPARATE";
    default: return "UNKNOWN_" + std::to_string(value);
  }
}

std::string photometric_to_string(std::uint16_t value)
{
  switch (value)
  {
    case PHOTOMETRIC_MINISWHITE: return "PHOTOMETRIC_MINISWHITE";
    case PHOTOMETRIC_MINISBLACK: return "PHOTOMETRIC_MINISBLACK";
    case PHOTOMETRIC_RGB: return "PHOTOMETRIC_RGB[A]";
    case PHOTOMETRIC_PALETTE: return "PHOTOMETRIC_PALETTE";
    case PHOTOMETRIC_MASK: return "PHOTOMETRIC_MASK";
    case PHOTOMETRIC_SEPARATED: return "PHOTOMETRIC_SEPARATED";
    case PHOTOMETRIC_YCBCR: return "PHOTOMETRIC_YCBCR";
    case PHOTOMETRIC_CIELAB: return "PHOTOMETRIC_CIELAB";
    case PHOTOMETRIC_ICCLAB: return "PHOTOMETRIC_ICCLAB";
    case PHOTOMETRIC_ITULAB: return "PHOTOMETRIC_ITULAB";
    case PHOTOMETRIC_CFA: return "PHOTOMETRIC_CFA";
    case PHOTOMETRIC_LOGL: return "PHOTOMETRIC_LOGL";
    case PHOTOMETRIC_LOGLUV: return "PHOTOMETRIC_LOGLUV";
    default: return "UNKNOWN_" + std::to_string(value);
  }
}

std::string sample_format_to_string(std::uint16_t value)
{
  switch (value)
  {
    case SAMPLEFORMAT_UINT: return "SAMPLEFORMAT_UINT";
    case SAMPLEFORMAT_INT: return "SAMPLEFORMAT_INT";
    case SAMPLEFORMAT_IEEEFP: return "SAMPLEFORMAT_IEEEFP";
    case SAMPLEFORMAT_VOID: return "SAMPLEFORMAT_VOID";
    case SAMPLEFORMAT_COMPLEXINT: return "SAMPLEFORMAT_COMPLEXINT";
    case SAMPLEFORMAT_COMPLEXIEEEFP: return "SAMPLEFORMAT_COMPLEXIEEEFP";
    default: return "UNKNOWN_" + std::to_string(value);
  }
}

std::string compression_to_string(std::uint16_t value)
{
  switch (value)
  {
    case COMPRESSION_NONE: return "COMPRESSION_NONE";
    case COMPRESSION_CCITTRLE: return "COMPRESSION_CCITTRLE";
    case COMPRESSION_CCITTFAX3: return "COMPRESSION_CCITTFAX3";
    case COMPRESSION_CCITTFAX4: return "COMPRESSION_CCITTFAX4";
    case COMPRESSION_LZW: return "COMPRESSION_LZW";
    case COMPRESSION_OJPEG: return "COMPRESSION_OJPEG";
    case COMPRESSION_JPEG: return "COMPRESSION_JPEG";
    case COMPRESSION_NEXT: return "COMPRESSION_NEXT";
    case COMPRESSION_CCITTRLEW: return "COMPRESSION_CCITTRLEW";
    case COMPRESSION_PACKBITS: return "COMPRESSION_PACKBITS";
    case COMPRESSION_THUNDERSCAN: return "COMPRESSION_THUNDERSCAN";
    case COMPRESSION_IT8CTPAD: return "COMPRESSION_IT8CTPAD";
    case COMPRESSION_IT8LW: return "COMPRESSION_IT8LW";
    case COMPRESSION_IT8MP: return "COMPRESSION_IT8MP";
    case COMPRESSION_IT8BL: return "COMPRESSION_IT8BL";
    case COMPRESSION_PIXARLOG: return "COMPRESSION_PIXARLOG";
    case COMPRESSION_DEFLATE: return "COMPRESSION_DEFLATE";
    case COMPRESSION_JBIG: return "COMPRESSION_JBIG";
    case COMPRESSION_LZMA: return "COMPRESSION_LZMA";
#if defined(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT)
    case COMPRESSION_ZSTD: return "COMPRESSION_ZSTD";
    case COMPRESSION_WEBP: return "COMPRESSION_WEBP";
#endif
    default: return "UNKNOWN_" + std::to_string(value);
  }
}

std::ostream& operator<<(std::ostream& os, const ImageLayoutStrips& info)
{
  os << "ImageLayoutStrips: nr_strips = " << info.nr_strips << ", size_bytes = " << info.size_bytes << ", rows_per_strip = " << info.rows_per_strip << '\n';
  return os;
}

std::ostream& operator<<(std::ostream& os, const ImageLayoutTiles& info)
{
  os << "ImageLayoutTiles: tile_size = (" << info.width << " x " << info.height << " x " << info.depth << "), size_bytes = " << info.size_bytes << '\n';
  return os;
}

std::ostream& operator<<(std::ostream& os, const YCbCrInfo& info)
{
  os << "YCbCrInfo: ycbcr_coeff = (" << info.coeff_red << ", " << info.coeff_green << ", " << info.coeff_blue << "), positioning = " << info.positioning << ", subsampling = [" << info.subsampling_horz << ", " << info.subsampling_vert << "]\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const OutputLayout& info)
{
  os << "OutputLayout: size = (" << info.width << " x " << info.height << "), nr_channels = " << info.nr_channels << ", nr_bytes_per_channel = " << info.nr_bytes_per_channel << '\n';
  os << "              pixel_format = " << info.pixel_format << ", sample_format = " << info.sample_format << "\n";
  return os;
}

std::uint8_t* copy_samples(const std::uint8_t* src_dense, std::size_t nr_src_pixels, std::size_t channel_offset,
                           std::size_t nr_bytes_per_channel, std::size_t nr_channels, std::uint8_t* dst)
{
  dst += channel_offset;
  const auto dst_offset = nr_bytes_per_channel * nr_channels;

  for (std::size_t s = 0; s < nr_src_pixels; ++s)
  {
    std::memcpy(dst, src_dense, nr_bytes_per_channel);
    src_dense += nr_bytes_per_channel;
    dst += dst_offset;
  }

  dst -= dst_offset;
  dst += nr_bytes_per_channel * nr_channels;
  return dst;
}

std::vector<std::uint8_t> convert_single_channel_1bit_to_8bit(const std::vector<std::uint8_t>& buf,
                                                              [[maybe_unused]] std::ptrdiff_t nr_bytes_read,
                                                              std::uint32_t width,
                                                              std::uint32_t height)
{
  assert(nr_bytes_read == width * height / 8);

  constexpr std::array<std::uint8_t, 2> res_arr = {{0x00, 0xFF}};

  auto transform_value = [&res_arr](const std::uint8_t val, std::uint8_t shift) {
    const auto index = (val & (std::uint8_t{0x01} << shift)) >> shift;
    return res_arr[index];
  };

  std::vector<std::uint8_t> out_buf(width * height);

  auto buf_ptr = buf.data();
  auto out_buf_ptr = out_buf.data();
  const auto out_buf_end = out_buf.data() + out_buf.size();

  while (out_buf_ptr < out_buf_end)
  {
    const auto val = *buf_ptr++;
    *out_buf_ptr++ = transform_value(val, 7);
    *out_buf_ptr++ = transform_value(val, 6);
    *out_buf_ptr++ = transform_value(val, 5);
    *out_buf_ptr++ = transform_value(val, 4);
    *out_buf_ptr++ = transform_value(val, 3);
    *out_buf_ptr++ = transform_value(val, 2);
    *out_buf_ptr++ = transform_value(val, 1);
    *out_buf_ptr++ = transform_value(val, 0);
  }

  assert(buf_ptr == buf.data() + nr_bytes_read);
  return out_buf;
}

std::vector<std::uint8_t> convert_single_channel_4bit_to_8bit(const std::vector<std::uint8_t>& buf,
                                                              [[maybe_unused]] std::ptrdiff_t nr_bytes_read,
                                                              std::uint32_t width,
                                                              std::uint32_t height)
{
  assert(nr_bytes_read == width * height / 2);

  constexpr std::array<std::uint8_t, 16> res_arr = {
      {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}};

  auto transform_value = [&res_arr](const std::uint8_t val, std::uint8_t shift) {
    const auto index = (val & (std::uint8_t{0x0F} << shift)) >> shift;
    return res_arr[index];
  };

  std::vector<std::uint8_t> out_buf(width * height);

  auto buf_ptr = buf.data();
  auto out_buf_ptr = out_buf.data();
  const auto out_buf_end = out_buf.data() + out_buf.size();

  while (out_buf_ptr < out_buf_end)
  {
    const auto val = *buf_ptr++;
    *out_buf_ptr++ = transform_value(val, 8);
    *out_buf_ptr++ = transform_value(val, 0);
  }

  assert(buf_ptr == buf.data() + nr_bytes_read);
  return out_buf;

}

std::vector<std::uint8_t> convert_ycbcr_to_rgb_interleaved(const std::vector<std::uint8_t>& buf,
                                                           [[maybe_unused]] std::ptrdiff_t nr_bytes_read,
                                                           std::uint32_t width,
                                                           std::uint32_t height,
                                                           const YCbCrInfo& ycbcr_info,
                                                           const YCbCrConverter& ycbcr_converter)
{
  // NB: assumes interleaved storage

  const auto sh = ycbcr_info.subsampling_horz;
  const auto sv = ycbcr_info.subsampling_vert;

  auto buf_ptr = buf.data();
  auto consume_buf = [&buf_ptr]() { return *buf_ptr++; };

  std::vector<std::uint8_t> out_buf(3 * width * height);
  sln::MutableImageView_8u3 out_img(out_buf.data(), {sln::to_pixel_length(width), sln::to_pixel_length(height)});

  std::vector<uint32> y_data_unit(sh * sv);
  for (std::uint32_t y = 0; y < height; y += sv)
  {
    for (std::uint32_t x = 0; x < width; x += sh)
    {
      // https://www.awaresystems.be/imaging/tiff/specification/TIFF6.pdf#page=93
      // Read "data unit" of sv * sh Y values, followed by Cb and Cr values
      std::generate(y_data_unit.begin(), y_data_unit.end(), consume_buf);
      const int32 Cb = consume_buf();
      const int32 Cr = consume_buf();

      assert(buf_ptr <= buf.data() + nr_bytes_read);

//      std::cout << "WRITING NEW MINIBATCH:\n";
      for (std::uint32_t dy = 0; dy < sv; ++dy)
      {
        for (std::uint32_t dx = 0; dx < sh; ++dx)
        {
          uint32 r = 0;
          uint32 g = 0;
          uint32 b = 0;
          const auto Y = y_data_unit[dy * sh + dx];
          ycbcr_converter.convert(Y, Cb, Cr, r, g, b);
//          std::cout << "  (" << Y << ", " << Cb << ", " << Cr << ") - >(" << r << ", " << g << ", " << b << ") @ [" << x + dx << ", " << y + dy << "]\n";
          out_img(sln::to_pixel_index(x + dx), sln::to_pixel_index(y + dy))
              = sln::Pixel_8u3(static_cast<std::uint8_t>(r),
                               static_cast<std::uint8_t>(g),
                               static_cast<std::uint8_t>(b));
        }
      }
    }
  }

  assert(buf_ptr == buf.data() + nr_bytes_read);
  return out_buf;
}

std::vector<std::uint8_t> convert_lab_to_rgb_interleaved(const std::vector<std::uint8_t>& buf,
                                                         [[maybe_unused]] std::ptrdiff_t nr_bytes_read,
                                                         std::uint32_t width,
                                                         std::uint32_t height,
                                                         const LabConverter& lab_converter)
{
  // NB: assumes interleaved storage
  assert(nr_bytes_read == 3 * width * height);

  std::vector<std::uint8_t> out_buf(3 * width * height);

  auto buf_ptr = buf.data();
  auto out_buf_ptr = out_buf.data();
  const auto out_buf_end = out_buf.data() + out_buf.size();
  while (out_buf_ptr < out_buf_end)
  {
    const uint32 lab_L = *buf_ptr++;
    const int32 lab_a = *buf_ptr++;
    const int32 lab_b = *buf_ptr++;
    uint32 r;
    uint32 g;
    uint32 b;
    lab_converter.convert(lab_L, lab_a, lab_b, r, g, b);
    *out_buf_ptr++ = static_cast<std::uint8_t>(r);
    *out_buf_ptr++ = static_cast<std::uint8_t>(g);
    *out_buf_ptr++ = static_cast<std::uint8_t>(b);
  }


  assert(buf_ptr == buf.data() + nr_bytes_read);
  return out_buf;
}


}  // namespace tiff
}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)
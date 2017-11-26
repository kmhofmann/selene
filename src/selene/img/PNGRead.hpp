// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_READ_HPP
#define SELENE_IMG_PNG_READ_HPP

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/MemoryReader.hpp>

#include <array>
#include <csetjmp>
#include <cstdio>
#include <memory>

namespace selene {
namespace img {

class PNGHeaderInfo;
struct PNGDecompressionOptions;
class PNGDecompressionObject;

namespace detail
{
class PNGDecompressionCycle;
void set_source(PNGDecompressionObject&, io::FileReader&);
void set_source(PNGDecompressionObject&, io::MemoryReader&);
PNGHeaderInfo read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
PNGHeaderInfo read_header(io::FileReader&, PNGDecompressionObject&);
PNGHeaderInfo read_header(io::MemoryReader&, PNGDecompressionObject&);
} // namespace detail


class PNGHeaderInfo
{
public:
  const Length width;
  const Length height;
  const int nr_channels;
  const int bit_depth;

  explicit PNGHeaderInfo(Length width_ = 0, Length height_ = 0, int nr_channels_ = 0, int bit_depth_ = 0);

  bool is_valid() const;
};

struct PNGDecompressionOptions
{
  bool force_bit_depth_8; // enforce a bit depth 8, for 16-bit inputs
  bool strip_alpha_channel; // remove a potential alpha channel (e.g. RGBA -> RGB, YA -> Y)
  bool swap_alpha_channel; // read ARGB instead of RGBA, for RGBA images
  bool set_bgr; // convert RGB to BGR
  bool invert_alpha_channel; // invert values in alpha channel (e.g. 0 -> 255)
  bool invert_monochrome; // invert grayscale or grayscale_alpha image values
  bool convert_gray_to_rgb; // convert grayscale images to RGB
  bool convert_rgb_to_gray; // convert RGB images to grayscale

  explicit PNGDecompressionOptions(bool force_bit_depth_8_ = false, bool strip_alpha_channel_ = false,
                                   bool swap_alpha_channel_ = false, bool set_bgr_ = false,
                                   bool invert_alpha_channel_ = false, bool invert_monochrome_ = false,
                                   bool convert_gray_to_rgb_ = false, bool convert_rgb_to_gray_ = false)
      : force_bit_depth_8(force_bit_depth_8_), strip_alpha_channel(strip_alpha_channel_),
        swap_alpha_channel(swap_alpha_channel_), set_bgr(set_bgr_), invert_alpha_channel(invert_alpha_channel_),
        invert_monochrome(invert_monochrome_), convert_gray_to_rgb(convert_gray_to_rgb_),
        convert_rgb_to_gray(convert_rgb_to_gray_)
  {
  }
};

class PNGDecompressionObject
{
public:
  PNGDecompressionObject();
  ~PNGDecompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  bool set_decompression_parameters(bool, bool, bool, bool, bool, bool, bool, bool);
  PixelFormat get_pixel_format() const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend class detail::PNGDecompressionCycle;
  friend void detail::set_source(PNGDecompressionObject&, io::FileReader&);
  friend void detail::set_source(PNGDecompressionObject&, io::MemoryReader&);
  friend PNGHeaderInfo detail::read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
};


template <typename SourceType>
PNGHeaderInfo read_png_header(SourceType& source, bool rewind = false, MessageLog* messages = nullptr);

template <typename SourceType>
PNGHeaderInfo read_png_header(PNGDecompressionObject& obj, SourceType& source, bool rewind = false,
                              MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_png(SourceType& source, PNGDecompressionOptions options = PNGDecompressionOptions(),
                   MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_png(PNGDecompressionObject& obj, SourceType& source,
                   PNGDecompressionOptions options = PNGDecompressionOptions(), MessageLog* messages = nullptr,
                   const PNGHeaderInfo* provided_header_info = nullptr);


//-----------------------------------------------------------------------------
// Implementation:

namespace detail
{

struct PNGOutputInfo
{
  Index width;
  Index height;
  int nr_channels;
  int bit_depth;
  std::size_t row_bytes;

  PNGOutputInfo();
  PNGOutputInfo(Index width_, Index height_, int nr_channels_, int bit_depth_, std::size_t row_bytes_);
};

class PNGDecompressionCycle
{
public:
  explicit PNGDecompressionCycle(PNGDecompressionObject& obj);
  ~PNGDecompressionCycle() = default;

  bool error_state() const;
  PNGOutputInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  PNGDecompressionObject& obj_;
  PNGOutputInfo output_info_;
  bool error_state_;
};

} // namespace detail

// ----------------
// Public functions

template <typename SourceType>
PNGHeaderInfo read_png_header(SourceType& source, bool rewind, MessageLog* messages)
{
  PNGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_png_header(obj, source, rewind, messages);
}

template <typename SourceType>
PNGHeaderInfo read_png_header(PNGDecompressionObject& obj, SourceType& source, bool rewind, MessageLog* messages)
{
  const auto src_pos = source.position();

  auto scope_exit = [&source, rewind, messages, &obj, src_pos]()
  {
    if (rewind)
    {
      source.seek_abs(src_pos);
    }

    detail::assign_message_log(obj, messages);
  };

  detail::set_source(obj, source);

  if (obj.error_state())
  {
    scope_exit();
    return PNGHeaderInfo();
  }

  const auto header_info = detail::read_header(source, obj);
  scope_exit();
  return header_info;

}

template <typename SourceType>
ImageData read_png(SourceType& source, PNGDecompressionOptions options, MessageLog* messages)
{
  PNGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_png(obj, source, options, messages);
}

template <typename SourceType>
ImageData read_png(PNGDecompressionObject& obj, SourceType& source, PNGDecompressionOptions options,
                   MessageLog* messages, const PNGHeaderInfo* provided_header_info)
{
  if (!provided_header_info)
  {
    detail::set_source(obj, source);

    if (obj.error_state())
    {
      detail::assign_message_log(obj, messages);
      return ImageData();
    }
  }

  const auto header_info = provided_header_info ? *provided_header_info : detail::read_header(source, obj);

  if (!header_info.is_valid())
  {
    detail::assign_message_log(obj, messages);
    return ImageData();
  }

  const bool pars_set = obj.set_decompression_parameters(options.force_bit_depth_8, options.strip_alpha_channel,
                                                         options.swap_alpha_channel, options.set_bgr,
                                                         options.invert_alpha_channel, options.invert_monochrome,
                                                         options.convert_gray_to_rgb, options.convert_rgb_to_gray);

  if (!pars_set)
  {
    detail::assign_message_log(obj, messages);
    return ImageData();
  }

  detail::PNGDecompressionCycle cycle(obj);

  if (cycle.error_state())
  {
    detail::assign_message_log(obj, messages);
    return ImageData();
  }

  const auto output_info = cycle.get_output_info();
  const auto output_width = output_info.width;
  const auto output_height = output_info.height;
  const auto output_nr_channels = output_info.nr_channels;
  const auto output_bit_depth = output_info.bit_depth;
  const auto output_nr_bytes_per_channel = output_bit_depth >> 3;
  const auto output_row_bytes = output_info.row_bytes;
  SELENE_FORCED_ASSERT(output_row_bytes == output_width * output_nr_channels * output_nr_bytes_per_channel);
  const auto output_pixel_format = obj.get_pixel_format();
  const auto output_sample_type = SampleType::UnsignedInteger;

  ImageData img;
  img.allocate(output_width, output_height, static_cast<std::uint16_t>(output_nr_channels),
               static_cast<std::uint8_t>(output_nr_bytes_per_channel), output_pixel_format, output_sample_type);
  auto row_pointers = get_row_pointers(img);
  const auto dec_success = cycle.decompress(row_pointers);

  if (!dec_success)
  {
    img.clear();  // invalidates image data
  }

  detail::assign_message_log(obj, messages);
  return img;
}

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

#endif // SELENE_IMG_PNG_READ_HPP
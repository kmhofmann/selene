// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_READ_HPP
#define SELENE_IMG_JPEG_READ_HPP

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/JPEGCommon.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/MemoryReader.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace selene {
namespace img {

struct JPEGHeaderInfo;
class JPEGDecompressionObject;

namespace detail
{
class JPEGDecompressionCycle;
void set_source(JPEGDecompressionObject&, io::FileReader&);
void set_source(JPEGDecompressionObject&, io::MemoryReader&);
JPEGHeaderInfo read_header(JPEGDecompressionObject&);
} // namespace detail

struct JPEGHeaderInfo
{
  const Index width;
  const Index height;
  const int nr_channels;
  const JPEGColorSpace color_space;

  explicit JPEGHeaderInfo(Index width_ = 0, Index height_ = 0, int nr_channels_ = 0,
                          JPEGColorSpace color_space_ = JPEGColorSpace::Unknown);

  bool is_valid() const;
};

struct JPEGDecompressionOptions
{
  JPEGColorSpace out_color_space;
  BoundingBox<Index> region;

  explicit JPEGDecompressionOptions(JPEGColorSpace out_color_space_ = JPEGColorSpace::Auto
#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
                                    , const BoundingBox<Index>& region_ = BoundingBox<Index>()
#endif
                                    )
      : out_color_space(out_color_space_)
#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
      , region(region_)
#endif
  {
  }
};

class JPEGDecompressionObject
{
public:
  JPEGDecompressionObject();
  ~JPEGDecompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  JPEGHeaderInfo get_header_info() const;
  void set_decompression_parameters(JPEGColorSpace out_color_space = JPEGColorSpace::Auto);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend class detail::JPEGDecompressionCycle;
  friend void detail::set_source(JPEGDecompressionObject&, io::FileReader&);
  friend void detail::set_source(JPEGDecompressionObject&, io::MemoryReader&);
  friend JPEGHeaderInfo detail::read_header(JPEGDecompressionObject&);
};

template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(SourceType& source, bool rewind = false, MessageLog* messages = nullptr);

template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(JPEGDecompressionObject& obj, SourceType& source, bool rewind = false,
                                MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_jpeg(SourceType& source, JPEGDecompressionOptions options = JPEGDecompressionOptions(),
               MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_jpeg(JPEGDecompressionObject& obj, SourceType& source,
               JPEGDecompressionOptions options = JPEGDecompressionOptions(), MessageLog* messages = nullptr,
               const JPEGHeaderInfo* provided_header_info = nullptr);


//-----------------------------------------------------------------------------
// Implementation:

namespace detail
{

struct JPEGOutputInfo
{
  const Index width;
  const Index height;
  const int nr_channels;

  JPEGOutputInfo(Index width_, Index height_, int nr_channels_);
};

class JPEGDecompressionCycle
{
public:
  JPEGDecompressionCycle(JPEGDecompressionObject& obj, const BoundingBox<Index>& region);

  ~JPEGDecompressionCycle();

  JPEGOutputInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  JPEGDecompressionObject& obj_;
  BoundingBox<Index> region_;
};

} // namespace detail

// ----------------
// Public functions

template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(SourceType& source, bool rewind, MessageLog* messages)
{
  JPEGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_jpeg_header(obj, source, rewind, messages);
}

template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(JPEGDecompressionObject& obj, SourceType& source, bool rewind, MessageLog* messages)
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
    return JPEGHeaderInfo();
  }

  const auto header_info = detail::read_header(obj);
  scope_exit();
  return header_info;
}

template <typename SourceType>
ImageData read_jpeg(SourceType& source, JPEGDecompressionOptions options, MessageLog* messages)
{
  JPEGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_jpeg(obj, source, options, messages);
}

template <typename SourceType>
ImageData read_jpeg(JPEGDecompressionObject& obj, SourceType& source, JPEGDecompressionOptions options,
                    MessageLog* messages, const JPEGHeaderInfo* provided_header_info)
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

  const auto header_info = provided_header_info ? *provided_header_info : detail::read_header(obj);

  if (!header_info.is_valid())
  {
    detail::assign_message_log(obj, messages);
    return ImageData();
  }

  obj.set_decompression_parameters(options.out_color_space);

  detail::JPEGDecompressionCycle cycle(obj, options.region);

  const auto output_info = cycle.get_output_info();
  const auto output_width = output_info.width;
  const auto output_height = options.region.empty() ? output_info.height : options.region.height();
  const auto output_nr_channels = static_cast<std::uint16_t>(output_info.nr_channels);
  const auto output_nr_bytes_per_channel = 1;
  const auto output_pixel_format = color_space_to_pixel_format(options.out_color_space);
  const auto output_sample_type = SampleType::UnsignedInteger;

  ImageData img;
  img.allocate(output_width, output_height, output_nr_channels, output_nr_bytes_per_channel, output_pixel_format,
               output_sample_type);
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

#endif // defined(SELENE_WITH_LIBJPEG)

#endif // SELENE_IMG_JPEG_READ_HPP
// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_WRITE_HPP
#define SELENE_IMG_PNG_WRITE_HPP

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryView.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileWriter.hpp>
#include <selene/io/VectorWriter.hpp>

#include <array>
#include <csetjmp>
#include <cstdio>
#include <memory>

namespace selene {
namespace img {

// Forward declarations

struct PNGCompressionOptions;
class PNGCompressionObject;

namespace detail
{
class PNGCompressionCycle;
void set_destination(PNGCompressionObject&, io::FileWriter&);
void set_destination(PNGCompressionObject&, io::VectorWriter&);
} // namespace detail


struct PNGCompressionOptions
{
  int compression_level;
  bool interlaced;
  bool set_bgr; // convert BGR (supplied) to RGB (written)
  bool invert_alpha_channel;  // invert values in alpha channel (e.g. 0 -> 255)
  bool invert_monochrome; // invert grayscale or grayscale_alpha image values

  explicit PNGCompressionOptions(int compression_level_ = -1, bool interlaced_ = false, bool set_bgr_ = false,
                                 bool invert_alpha_channel_ = false, bool invert_monochrome_ = false)
      : compression_level(compression_level_), interlaced(interlaced_), set_bgr(set_bgr_),
        invert_alpha_channel(invert_alpha_channel_), invert_monochrome(invert_monochrome_)
  {
  }
};

class PNGCompressionObject
{
public:
  PNGCompressionObject();
  ~PNGCompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  bool set_image_info(int width, int height, int nr_channels, int bit_depth, bool interlaced, PixelFormat pixel_format);
  bool set_compression_parameters(int, bool);

private:
  struct Impl;
  std::unique_ptr <Impl> impl_;

  friend class detail::PNGCompressionCycle;
  friend void detail::set_destination(PNGCompressionObject&, io::FileWriter&);
  friend void detail::set_destination(PNGCompressionObject&, io::VectorWriter&);
};


template <typename SinkType>
bool write_png(SinkType& sink, const ImageData& img_data, PNGCompressionOptions options = PNGCompressionOptions(),
               MessageLog* messages = nullptr);

template <typename SinkType>
bool write_png(PNGCompressionObject& obj, SinkType& sink, const ImageData& img_data,
               PNGCompressionOptions options = PNGCompressionOptions(), MessageLog* messages = nullptr);


//-----------------------------------------------------------------------------
// Implementation:

namespace detail
{

class PNGCompressionCycle
{
public:
  explicit PNGCompressionCycle(PNGCompressionObject& obj, bool, bool);
  ~PNGCompressionCycle() = default;

  bool error_state() const;
  void compress(const ConstRowPointers& row_pointers);

private:
  PNGCompressionObject& obj_;
  bool error_state_;
};

} // namespace detail

// ----------------
// Public functions

template <typename SinkType>
bool write_png(SinkType& sink, const ImageData& img_data, PNGCompressionOptions options, MessageLog* messages)
{
  PNGCompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return write_png(obj, sink, img_data, options, messages);
};

template <typename SinkType>
bool write_png(PNGCompressionObject& obj, SinkType& sink, const ImageData& img_data, PNGCompressionOptions options,
               MessageLog* messages)
{
  if (img_data.nr_bytes_per_channel() != 1 && img_data.nr_bytes_per_channel() != 2)
  {
    throw std::runtime_error("Unsupported bit depth of image data for PNG output");
  }

  detail::set_destination(obj, sink);

  if (obj.error_state())
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  const auto nr_channels = img_data.nr_channels();
  const auto bit_depth = img_data.nr_bytes_per_channel() == 1 ? 8 : 16;

  const bool img_info_set = obj.set_image_info(img_data.width(), img_data.height(), nr_channels, bit_depth,
                                               options.interlaced, img_data.pixel_format());

  if (!img_info_set)
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  const bool pars_set = obj.set_compression_parameters(options.compression_level, options.invert_alpha_channel);

  if (!pars_set)
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  detail::PNGCompressionCycle cycle(obj, options.set_bgr, options.invert_monochrome);
  const auto row_pointers = get_row_pointers(img_data);
  cycle.compress(row_pointers);

  detail::assign_message_log(obj, messages);
  return !obj.error_state();
}

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

#endif // SELENE_IMG_PNG_WRITE_HPP
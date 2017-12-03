// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Utils.hpp>
#include <selene/img/PNGWrite.hpp>
#include <selene/img/detail/PNGDetail.hpp>

#include <png.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

namespace selene {
namespace img {

namespace {

constexpr int PNG_COLOR_TYPE_INVALID = -1;

int determine_color_type(PixelFormat pixel_format)
{
  switch (pixel_format)
  {
    case PixelFormat::Y: return PNG_COLOR_TYPE_GRAY;
    case PixelFormat::X: return PNG_COLOR_TYPE_GRAY;
    case PixelFormat::YA: return PNG_COLOR_TYPE_GRAY_ALPHA;
    case PixelFormat::XX: return PNG_COLOR_TYPE_GRAY_ALPHA;
    case PixelFormat::RGB: return PNG_COLOR_TYPE_RGB;
    case PixelFormat::BGR: return PNG_COLOR_TYPE_RGB;
    case PixelFormat::XXX: return PNG_COLOR_TYPE_RGB;
    case PixelFormat::RGBA: return PNG_COLOR_TYPE_RGBA;
    case PixelFormat::BGRA: return PNG_COLOR_TYPE_RGBA;
    case PixelFormat::XXXX: return PNG_COLOR_TYPE_RGBA;
    default:
      return PNG_COLOR_TYPE_INVALID;
  }
}

} // namespace _

/// \cond INTERNAL

struct PNGCompressionObject::Impl
{
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  detail::PNGErrorManager error_manager;
  bool valid = false;
};

PNGCompressionObject::PNGCompressionObject()
    : impl_(std::make_unique<PNGCompressionObject::Impl>())
{
  auto user_error_ptr = static_cast<png_voidp>(&impl_->error_manager);
  png_error_ptr user_error_fn = detail::error_handler;
  png_error_ptr user_warning_fn = detail::warning_handler;
  impl_->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, user_error_ptr, user_error_fn, user_warning_fn);

  if (!impl_->png_ptr)
  {
    return;
  }

  impl_->info_ptr = png_create_info_struct(impl_->png_ptr);

  if (!impl_->info_ptr)
  {
    png_destroy_write_struct(&impl_->png_ptr, (png_infopp)nullptr);
    return;
  }

  SELENE_FORCED_ASSERT(impl_->png_ptr);
  SELENE_FORCED_ASSERT(impl_->info_ptr);
  impl_->valid = true;
}

PNGCompressionObject::~PNGCompressionObject()
{
  png_destroy_write_struct(&impl_->png_ptr, &impl_->info_ptr);
}

bool PNGCompressionObject::valid() const
{
  return impl_->valid;
}

bool PNGCompressionObject::set_image_info(int width, int height, int nr_channels, int bit_depth, bool interlaced,
                                          PixelFormat pixel_format)
{
  auto png_ptr = impl_->png_ptr;
  auto info_ptr = impl_->info_ptr;

  const auto color_type = determine_color_type(pixel_format);
  const auto interlace_type = interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  const auto compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
  const auto filter_method = PNG_FILTER_TYPE_DEFAULT;

  if (color_type == PNG_COLOR_TYPE_INVALID)
  {
    impl_->error_manager.message_log.add_message("Cannot determine PNG color type from pixel format of image data");
    return false;
  }

  if ((color_type == PNG_COLOR_TYPE_GRAY && nr_channels != 1) ||
      (color_type == PNG_COLOR_TYPE_GRAY_ALPHA && nr_channels != 2) ||
      (color_type == PNG_COLOR_TYPE_RGB && nr_channels != 3) ||
      (color_type == PNG_COLOR_TYPE_RGB_ALPHA && nr_channels != 4))
  {
    impl_->error_manager.message_log.add_message("Mismatch between determined PNG color type and nr of channels");
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  // TODO: Check for (dis)allowed combinations of bit_depth and color_type
  // ...

  png_set_IHDR(png_ptr, info_ptr, static_cast<png_uint_32>(width), static_cast<png_uint_32>(height), bit_depth,
               color_type, interlace_type, compression_type, filter_method);

  return true;

failure_state:
  return false;
}

bool PNGCompressionObject::set_compression_parameters(int compression_level, bool invert_alpha)
{
  auto png_ptr = impl_->png_ptr;

  compression_level = clamp(compression_level, 0, 9);

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  png_set_compression_level(png_ptr, compression_level);

  if (invert_alpha)
  {
    png_set_invert_alpha(png_ptr);
  }

  return true;

failure_state:
  return false;
}

bool PNGCompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

const MessageLog& PNGCompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}


namespace detail
{

// ----------------------
// Compression structures

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

PNGCompressionCycle::PNGCompressionCycle(PNGCompressionObject& obj, bool set_bgr,
                                         bool invert_monochrome)
    : obj_(obj), error_state_(false)
{
  auto png_ptr = obj_.impl_->png_ptr;
  auto info_ptr = obj_.impl_->info_ptr;

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  png_write_info(png_ptr, info_ptr);

  if (set_bgr)
  {
    png_set_bgr(png_ptr);
  }

  if (invert_monochrome)
  {
    png_set_invert_mono(png_ptr);
  }

  return;

failure_state:
  error_state_ = true;
}

bool PNGCompressionCycle::error_state() const
{
  return error_state_;
}

void PNGCompressionCycle::compress(const ConstRowPointers& row_pointers)
{
  auto png_ptr = obj_.impl_->png_ptr;
  auto info_ptr = obj_.impl_->info_ptr;

  // Hack to accommodate non-const correct API
  const auto png_row_pointers = const_cast<png_bytepp>(row_pointers.data());
  png_write_image(png_ptr, png_row_pointers);

  png_write_end(png_ptr, info_ptr);
}


void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  void* io_ptr = png_get_io_ptr(png_ptr);

  if (io_ptr == nullptr)
  {
    detail::error_handler(png_ptr, "[selene] png_get_io_ptr() failed");
  }

  auto writer = static_cast<io::VectorWriter*>(io_ptr);
  SELENE_ASSERT(writer);

  auto nr_bytes_written = io::write(*writer, data, length);
  SELENE_FORCED_ASSERT(nr_bytes_written == length);
}

void user_flush_data(png_structp /*png_ptr*/)
{
}

void set_destination(PNGCompressionObject& obj, io::FileWriter& sink)
{
  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_init_io(obj.impl_->png_ptr, sink.handle());

failure_state:;
}

void set_destination(PNGCompressionObject& obj, io::VectorWriter& sink)
{
  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_set_write_fn(obj.impl_->png_ptr, static_cast<png_voidp>(&sink), user_write_data, user_flush_data);

failure_state:;
}

} // namespace detail

/// \endcond


// ----------------
// Public functions

template <typename SinkType>
bool write_png(const ImageData& img_data, SinkType& sink, PNGCompressionOptions options, MessageLog* messages)
{
  PNGCompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return write_png(img_data, obj, sink, options, messages);
};

template <typename SinkType>
bool write_png(const ImageData& img_data, PNGCompressionObject& obj, SinkType& sink, PNGCompressionOptions options,
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

  const bool img_info_set = obj.set_image_info(static_cast<int>(img_data.width()), static_cast<int>(img_data.height()),
                                               static_cast<int>(nr_channels), static_cast<int>(bit_depth),
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


// ----------
// Explicit instantiations:

template bool write_png<io::FileWriter>(const ImageData&, io::FileWriter&, PNGCompressionOptions, MessageLog*);
template bool write_png<io::VectorWriter>(const ImageData&, io::VectorWriter&, PNGCompressionOptions, MessageLog*);

template bool write_png<io::FileWriter>(const ImageData&, PNGCompressionObject&, io::FileWriter&,
                                         PNGCompressionOptions, MessageLog*);
template bool write_png<io::VectorWriter>(const ImageData&, PNGCompressionObject&, io::VectorWriter&,
                                           PNGCompressionOptions, MessageLog*);

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBPNG)

#include <png.h>

#include <selene/base/Utils.hpp>

#include <selene/img_io/png/Write.hpp>
#include <selene/img_io/png/_impl/Detail.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

namespace sln {

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
    default: return PNG_COLOR_TYPE_INVALID;
  }
}

}  // namespace

/// \cond INTERNAL

struct PNGCompressionObject::Impl
{
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  impl::PNGErrorManager error_manager;
  bool valid = false;
  bool needs_reset = false;
};

PNGCompressionObject::PNGCompressionObject() : impl_(std::make_unique<PNGCompressionObject::Impl>())
{
  auto user_error_ptr = static_cast<png_voidp>(&impl_->error_manager);
  png_error_ptr user_error_fn = impl::error_handler;
  png_error_ptr user_warning_fn = impl::warning_handler;
  impl_->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, user_error_ptr, user_error_fn, user_warning_fn);

  if (!impl_->png_ptr)
  {
    return;
  }

  impl_->info_ptr = png_create_info_struct(impl_->png_ptr);

  if (!impl_->info_ptr)
  {
    png_destroy_write_struct(&impl_->png_ptr, (png_infopp) nullptr);
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

bool PNGCompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

MessageLog& PNGCompressionObject::message_log()
{
  return impl_->error_manager.message_log;
}

const MessageLog& PNGCompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}

bool PNGCompressionObject::set_image_info(
    int width, int height, int nr_channels, int bit_depth, bool interlaced, PixelFormat pixel_format)
{
  auto png_ptr = impl_->png_ptr;
  auto info_ptr = impl_->info_ptr;

  const auto color_type = determine_color_type(pixel_format);
  const auto interlace_type = interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  const auto compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
  const auto filter_method = PNG_FILTER_TYPE_DEFAULT;

  if (color_type == PNG_COLOR_TYPE_INVALID)
  {
    impl_->error_manager.message_log.add(
        "Cannot determine PNG color type from pixel format of image data",
        MessageType::Error);
    return false;
  }

  if ((color_type == PNG_COLOR_TYPE_GRAY && nr_channels != 1)
      || (color_type == PNG_COLOR_TYPE_GRAY_ALPHA && nr_channels != 2)
      || (color_type == PNG_COLOR_TYPE_RGB && nr_channels != 3)
      || (color_type == PNG_COLOR_TYPE_RGB_ALPHA && nr_channels != 4))
  {
    impl_->error_manager.message_log.add(
        "Mismatch between determined PNG color type and nr of channels",
        MessageType::Error);
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

bool PNGCompressionObject::set_compression_parameters(int compression_level,
                                                      bool invert_alpha_channel)
{
  auto png_ptr = impl_->png_ptr;

  compression_level = std::clamp(compression_level, 0, 9);

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  png_set_compression_level(png_ptr, compression_level);

  if (invert_alpha_channel)
  {
    png_set_invert_alpha(png_ptr);
  }

  return true;

failure_state:
  return false;
}

void PNGCompressionObject::reset_if_needed()
{
  if (impl_->needs_reset)
  {
    // TODO: do we also need to deallocate and reallocate the png structs here, as in the decompression object?
    impl_->error_manager.error_state = false;
    impl_->error_manager.message_log.clear();
    impl_->needs_reset = false;
  }
}


namespace impl {

// ----------------------
// Compression structures

PNGCompressionCycle::PNGCompressionCycle(PNGCompressionObject& obj,
                                         bool set_bgr,
                                         bool invert_monochrome,
                                         bool keep_endianness,
                                         int bit_depth)
    : obj_(obj), error_state_(false)
{
  obj_.reset_if_needed();

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

  if (bit_depth > 8 && !keep_endianness)
  {
    png_set_swap(png_ptr);
  }

  return;

failure_state:
  error_state_ = true;
}

PNGCompressionCycle::~PNGCompressionCycle()
{
  obj_.impl_->needs_reset = true;
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
    impl::error_handler(png_ptr, "[selene] png_get_io_ptr() failed");
  }

  auto writer = static_cast<VectorWriter*>(io_ptr);
  SELENE_ASSERT(writer);

  auto nr_bytes_written = write(*writer, data, length);
  SELENE_FORCED_ASSERT(nr_bytes_written == length);
}

void user_flush_data(png_structp /*png_ptr*/)
{
}

void set_destination(PNGCompressionObject& obj, FileWriter& sink)
{
  obj.reset_if_needed();

  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_init_io(obj.impl_->png_ptr, sink.handle());

failure_state:;
}

void set_destination(PNGCompressionObject& obj, VectorWriter& sink)
{
  obj.reset_if_needed();

  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_set_write_fn(obj.impl_->png_ptr, static_cast<png_voidp>(&sink), user_write_data, user_flush_data);

failure_state:;
}

}  // namespace impl

/// \endcond

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBPNG)

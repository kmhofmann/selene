// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <selene/img/PNGRead.hpp>
#include <selene/img/detail/PNGDetail.hpp>

#include <png.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

namespace selene {
namespace img {

PNGHeaderInfo::PNGHeaderInfo(Length width_, Length height_, int nr_channels_, int bit_depth_)
    : width(width_), height(height_), nr_channels(nr_channels_), bit_depth(bit_depth_)
{
}

bool PNGHeaderInfo::is_valid() const
{
  return width > 0 && height > 0 && nr_channels > 0 && bit_depth > 0;
}

//// ----------

struct PNGDecompressionObject::Impl
{
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  png_infop end_info = nullptr;
  detail::PNGErrorManager error_manager;
  PixelFormat pixel_format_ = PixelFormat::Unknown;
  bool valid = false;
};

PNGDecompressionObject::PNGDecompressionObject()
    : impl_(std::make_unique<PNGDecompressionObject::Impl>())
{
  auto user_error_ptr = static_cast<png_voidp>(&impl_->error_manager);
  png_error_ptr user_error_fn = detail::error_handler;
  png_error_ptr user_warning_fn = detail::warning_handler;
  impl_->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, user_error_ptr, user_error_fn, user_warning_fn);

  if (!impl_->png_ptr)
  {
    return;
  }

  impl_->info_ptr = png_create_info_struct(impl_->png_ptr);

  if (!impl_->info_ptr)
  {
    png_destroy_read_struct(&impl_->png_ptr, (png_infopp)nullptr, (png_infopp)nullptr);
    return;
  }

  impl_->end_info = png_create_info_struct(impl_->png_ptr);

  if (!impl_->end_info)
  {
    png_destroy_read_struct(&impl_->png_ptr, &impl_->info_ptr, (png_infopp)nullptr);
    return;
  }

  SELENE_FORCED_ASSERT(impl_->png_ptr);
  SELENE_FORCED_ASSERT(impl_->info_ptr);
  SELENE_FORCED_ASSERT(impl_->end_info);
  impl_->valid = true;
}

PNGDecompressionObject::~PNGDecompressionObject()
{
  // Deallocate the memory allocated for the png_struct and png_info objects
  png_destroy_read_struct(&impl_->png_ptr, &impl_->info_ptr, &impl_->end_info);
}

bool PNGDecompressionObject::valid() const
{
  return impl_->valid;
}

bool PNGDecompressionObject::set_decompression_parameters(bool force_bit_depth_8, bool strip_alpha_channel,
                                                          bool swap_alpha_channel, bool set_bgr,
                                                          bool invert_alpha_channel, bool invert_monochrome,
                                                          bool convert_gray_to_rgb, bool convert_rgb_to_gray)
{
  auto png_ptr = impl_->png_ptr;
  auto info_ptr = impl_->info_ptr;

  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_method = 0;
  int compression_method = 0;
  int filter_method = 0;

  //png_byte nr_channels = 0;
  //png_size_t rowbytes = 0;
  //png_const_bytep signature = 0;

  double screen_gamma = PNG_DEFAULT_sRGB; // TODO: Or supply user-defined value.

  // Unfortunately, no more variable initializations after this line:
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  png_set_compression_buffer_size(png_ptr, 4 * 8192); // Default is 8192
  //png_set_crc_action(png_ptr, crit_action, ancil_action);
  // TODO: Set up callback for unknown chunks? See line 492 in manual
  //png_set_read_status_fn(png_ptr, read_row_callback);

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method,
               &filter_method);

  png_set_packing(png_ptr);

  if (force_bit_depth_8 && bit_depth == 16)
  {
    png_set_scale_16(png_ptr);
  }

  switch (color_type)
  {
    case PNG_COLOR_TYPE_GRAY: impl_->pixel_format_ = PixelFormat::Y; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: impl_->pixel_format_ = PixelFormat::YA; break;
    case PNG_COLOR_TYPE_RGB: impl_->pixel_format_ = PixelFormat::RGB; break;
    case PNG_COLOR_TYPE_RGBA: impl_->pixel_format_ = PixelFormat::RGBA; break;
    default: impl_->pixel_format_ = PixelFormat::Unknown; break;
  }

  // - Strip the alpha channel, if desired
  if (strip_alpha_channel && (color_type & PNG_COLOR_MASK_ALPHA))
  {
    png_set_strip_alpha(png_ptr);

    if (impl_->pixel_format_ == PixelFormat::YA)
    {
      impl_->pixel_format_ = PixelFormat::Y;
    }
    else if (impl_->pixel_format_ == PixelFormat::RGBA)
    {
      impl_->pixel_format_ = PixelFormat::RGB;
    }
  }

  // - Convert palette images to RGB
  if (color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_set_palette_to_rgb(png_ptr);
    impl_->pixel_format_ = PixelFormat::RGB;
  }

  // - Expand bit depths < 8 bits to 8 bits
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
  {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  }

  // Expand paletted or RGB images with transparency to full alpha channels, so the data will be available as RGBA
  // quartets.
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0)
  {
    png_set_tRNS_to_alpha(png_ptr);

    if (impl_->pixel_format_ == PixelFormat::RGB)
    {
      impl_->pixel_format_ = PixelFormat::RGBA;
    }
  }

  // Set the background color to draw transparent and alpha images over.
  png_color_16 my_background, *image_background;

  if (png_get_bKGD(png_ptr, info_ptr, &image_background) != 0)
  {
    png_set_background(png_ptr, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
  }
  else
  {
    png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
  }

  int intent;

  if (png_get_sRGB(png_ptr, info_ptr, &intent) != 0)
  {
    png_set_gamma(png_ptr, screen_gamma, PNG_DEFAULT_sRGB);
  }
  else
  {
    double image_gamma;

    if (png_get_gAMA(png_ptr, info_ptr, &image_gamma) != 0)
    {
      png_set_gamma(png_ptr, screen_gamma, image_gamma);
    }
    else
    {
      png_set_gamma(png_ptr, screen_gamma, 0.45455);
    }
  }

  if (swap_alpha_channel)
  {
    png_set_invert_alpha(png_ptr);
  }

  if (set_bgr && (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA))
  {
    png_set_bgr(png_ptr);

    if (impl_->pixel_format_ == PixelFormat::RGB)
    {
      impl_->pixel_format_ = PixelFormat::BGR;
    }
    else if (impl_->pixel_format_ == PixelFormat::RGBA)
    {
      impl_->pixel_format_ = PixelFormat::BGRA;
    }
  }

  if (invert_alpha_channel && color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
    png_set_swap_alpha(png_ptr);

    if (impl_->pixel_format_ == PixelFormat::RGBA)
    {
      impl_->pixel_format_ = PixelFormat::ARGB;
    }
    else if (impl_->pixel_format_ == PixelFormat::BGRA)
    {
      impl_->pixel_format_ = PixelFormat::ABGR;
    }
  }

  if (convert_gray_to_rgb && (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
  {
    png_set_gray_to_rgb(png_ptr);

    if (impl_->pixel_format_ == PixelFormat::Y)
    {
      impl_->pixel_format_ = PixelFormat::RGB;
    }
    else if (impl_->pixel_format_ == PixelFormat::YA)
    {
      impl_->pixel_format_ = PixelFormat::RGBA;
    }
  }

  if (convert_rgb_to_gray && (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA))
  {
    constexpr int error_action = PNG_ERROR_ACTION_NONE; // silently do the conversion
    png_set_rgb_to_gray(png_ptr, error_action, PNG_RGB_TO_GRAY_DEFAULT, PNG_RGB_TO_GRAY_DEFAULT);

    if (impl_->pixel_format_ == PixelFormat::RGB)
    {
      impl_->pixel_format_ = PixelFormat::Y;
    }
    else if (impl_->pixel_format_ == PixelFormat::RGBA)
    {
      impl_->pixel_format_ = PixelFormat::YA;
    }
  }

  if (invert_monochrome && (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
  {
    png_set_invert_mono(png_ptr);
  }

  png_set_interlace_handling(png_ptr);

  return true;

failure_state:
  return false;
}

PixelFormat PNGDecompressionObject::get_pixel_format() const
{
  return impl_->pixel_format_;
}

bool PNGDecompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

const MessageLog& PNGDecompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}

// ----------

namespace detail
{

PNGOutputInfo::PNGOutputInfo()
    : width(0), height(0), nr_channels(0), bit_depth(0), row_bytes(0)
{
}

PNGOutputInfo::PNGOutputInfo(Length width_, Length height_, int nr_channels_, int bit_depth_, std::size_t row_bytes_)
    : width(width_), height(height_), nr_channels(nr_channels_), bit_depth(bit_depth_), row_bytes(row_bytes_)
{
}

// ------------------------
// Decompression structures

PNGDecompressionCycle::PNGDecompressionCycle(PNGDecompressionObject& obj)
    : obj_(obj), error_state_(false)
{
  auto png_ptr = obj_.impl_->png_ptr;
  auto info_ptr = obj_.impl_->info_ptr;

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  png_read_update_info(png_ptr, info_ptr);

  output_info_ = PNGOutputInfo(
      static_cast<Length>(png_get_image_width(png_ptr, info_ptr)),
      static_cast<Length>(png_get_image_height(png_ptr, info_ptr)),
      static_cast<int>(png_get_channels(png_ptr, info_ptr)),
      static_cast<int>(png_get_bit_depth(png_ptr, info_ptr)),
      static_cast<std::size_t>(png_get_rowbytes(png_ptr, info_ptr)));

  //color_type = png_get_color_type(png_ptr, info_ptr);

  if (output_info_.bit_depth != 8 && output_info_.bit_depth != 16) // bit depths 1, 2, 4 should be converted using above transformation
  {
    error_state_ = true;
  }

  return;

failure_state:
  error_state_ = true;
}

bool PNGDecompressionCycle::error_state() const
{
  return error_state_;
}

PNGOutputInfo PNGDecompressionCycle::get_output_info() const
{
  return output_info_;
}

bool PNGDecompressionCycle::decompress(RowPointers& row_pointers)
{
  auto png_ptr = obj_.impl_->png_ptr;
  auto end_info = obj_.impl_->end_info;

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    goto failure_state;
  }

  // Read PNG image in one go
  png_read_image(png_ptr, row_pointers.data());
  // Read comment or time chunks
  // TODO: Make use of these

  png_read_end(png_ptr, end_info);
  return true;

failure_state:
  return false;
}

// -------------------------------
// Decompression related functions

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  void* io_ptr = png_get_io_ptr(png_ptr);

  if (io_ptr == nullptr)
  {
    detail::error_handler(png_ptr, "[selene] png_get_io_ptr() failed");
  }

  auto reader = static_cast<io::MemoryReader*>(io_ptr);
  SELENE_ASSERT(reader);

  if (static_cast<png_size_t>(reader->bytes_remaining()) < length)
  {
    detail::error_handler(png_ptr, "[selene] access in user_read_data() out of bounds");
  }

  auto nr_bytes_read = io::read(*reader, data, length);
  SELENE_FORCED_ASSERT(nr_bytes_read == length);
}

void set_source(PNGDecompressionObject& obj, io::FileReader& source)
{
  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_init_io(obj.impl_->png_ptr, source.handle());

  failure_state:;
}

void set_source(PNGDecompressionObject& obj, io::MemoryReader& source)
{
  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_set_read_fn(obj.impl_->png_ptr, static_cast<png_voidp>(&source), user_read_data);

failure_state:;
}

PNGHeaderInfo read_header_info(PNGDecompressionObject& obj, const std::array<std::uint8_t, 8>& header_bytes, bool eof)
{
  auto png_ptr = obj.impl_->png_ptr;
  auto info_ptr = obj.impl_->info_ptr;
  auto& message_log = obj.impl_->error_manager.message_log;

  auto signature_correct = png_sig_cmp(header_bytes.data(), 0, 8);
  bool error = eof || (signature_correct != 0);
  png_set_sig_bytes(png_ptr, 8);

  if (error)
  {
    message_log.add_message("Source is not a PNG file.");
    return PNGHeaderInfo();
  }

  Index width = 0;
  Index height = 0;
  int bit_depth = 0;
  int nr_channels = 0;

  if (setjmp(png_jmpbuf(obj.impl_->png_ptr)))
  {
    goto failure_state;
  }

  png_read_info(png_ptr, info_ptr);

  width = static_cast<Length>(png_get_image_width(png_ptr, info_ptr));
  height = static_cast<Length>(png_get_image_height(png_ptr, info_ptr));
  bit_depth = static_cast<int>(png_get_bit_depth(png_ptr, info_ptr));
  nr_channels = static_cast<int>(png_get_channels(png_ptr, info_ptr));

  return PNGHeaderInfo(width, height, nr_channels, bit_depth);

failure_state:
  return PNGHeaderInfo();
}

PNGHeaderInfo read_header(io::FileReader& source, PNGDecompressionObject& obj)
{
  // Check if the file is a PNG file (look at first 8 bytes)
  std::array<std::uint8_t, 8> header_bytes;
  source.template read<std::uint8_t>(header_bytes.data(), 8);

  return read_header_info(obj, header_bytes, source.is_eof());
}

PNGHeaderInfo read_header(io::MemoryReader& source, PNGDecompressionObject& obj)
{
  // Check if the file is a PNG file (look at first 8 bytes)
  std::array<std::uint8_t, 8> header_bytes;
  source.template read<std::uint8_t>(header_bytes.data(), 8);

  return read_header_info(obj, header_bytes, source.is_eof());
}

} // namespace detail

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

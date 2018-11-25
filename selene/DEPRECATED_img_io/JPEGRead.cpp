// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/DEPRECATED_img_io/JPEGRead.hpp>
#include <selene/DEPRECATED_img_io/impl/JPEGDetail.hpp>

#include <jpeglib.h>

#include <cstdio>
#include <stdexcept>

namespace sln {

/** \brief Constructor. Instantiates a JPEGHeaderInfo object with the specified parameters.
 *
 * @param width_ The image width.
 * @param height_ The image height.
 * @param nr_channels_ The number of image channels.
 * @param color_space_ The image data color space.
 */
JPEGImageInfo::JPEGImageInfo(PixelLength width_, PixelLength height_, std::uint16_t nr_channels_, JPEGColorSpace color_space_)
    : width(width_), height(height_), nr_channels(nr_channels_), color_space(color_space_)
{
}

/** \brief Returns whether the contained JPEG header information is valid.
 *
 * @return True, if the header information is valid; false otherwise.
 */
bool JPEGImageInfo::is_valid() const
{
  return width > 0 && height > 0 && nr_channels > 0;
}

/// \cond INTERNAL

struct JPEGDecompressionObject::Impl
{
  jpeg_decompress_struct cinfo;
  impl::JPEGErrorManager error_manager;
  bool valid = false;
  bool needs_reset = false;
};

JPEGDecompressionObject::JPEGDecompressionObject() : impl_(std::make_unique<JPEGDecompressionObject::Impl>())
{
  impl_->cinfo.err = jpeg_std_error(&impl_->error_manager.pub);
  impl_->cinfo.err->error_exit = impl::error_exit;
  impl_->cinfo.err->output_message = impl::output_message;
  jpeg_create_decompress(&impl_->cinfo);
  impl_->valid = true;
}

JPEGDecompressionObject::~JPEGDecompressionObject()
{
  jpeg_destroy_decompress(&impl_->cinfo);
}

void JPEGDecompressionObject::reset_if_needed()
{
  if (impl_->needs_reset)
  {
    impl_->error_manager.error_state = false;
    impl_->error_manager.message_log.clear();
    impl_->needs_reset = false;
  }
}

bool JPEGDecompressionObject::valid() const
{
  return impl_->valid;
}

JPEGImageInfo JPEGDecompressionObject::get_header_info() const
{
  const auto color_space = impl::color_space_lib_to_pub(impl_->cinfo.jpeg_color_space);
  return JPEGImageInfo(PixelLength(impl_->cinfo.image_width), PixelLength(impl_->cinfo.image_height),
                       static_cast<std::uint16_t>(impl_->cinfo.num_components), color_space);
}

void JPEGDecompressionObject::set_decompression_parameters(JPEGColorSpace out_color_space)
{
  if (out_color_space != JPEGColorSpace::Auto)
  {
    impl_->cinfo.out_color_space = impl::color_space_pub_to_lib(out_color_space);
  }
}

bool JPEGDecompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

MessageLog& JPEGDecompressionObject::message_log()
{
  return impl_->error_manager.message_log;
}

const MessageLog& JPEGDecompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}


namespace impl {

JPEGDecompressionCycle::JPEGDecompressionCycle(JPEGDecompressionObject& obj, const BoundingBox& region)
    : obj_(obj), region_(region)
{
  obj_.reset_if_needed();

  auto& cinfo = obj_.impl_->cinfo;

  jpeg_start_decompress(&cinfo);

  if (!region_.empty())
  {
    region_.sanitize(PixelLength(cinfo.output_width), PixelLength(cinfo.output_height));
  }

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
  if (!region_.empty())
  {
    // Enable partial decompression of each scanline
    auto xoffset = static_cast<JDIMENSION>(region_.x0());
    auto width = static_cast<JDIMENSION>(region_.width());
    jpeg_crop_scanline(&cinfo, &xoffset, &width);
  }
#endif
}

JPEGDecompressionCycle::~JPEGDecompressionCycle()
{
  if (!finished_or_aborted_)
  {
    auto& cinfo = obj_.impl_->cinfo;
    jpeg_abort_decompress(&cinfo);
  }

  obj_.impl_->needs_reset = true;
}

JPEGImageInfo JPEGDecompressionCycle::get_output_info() const
{
  auto& cinfo = obj_.impl_->cinfo;
  SELENE_FORCED_ASSERT(cinfo.out_color_components == cinfo.output_components);

  const auto width = PixelLength(cinfo.output_width);
  const auto height = region_.empty() ? PixelLength(cinfo.output_height) : PixelLength(region_.height());
  const auto out_color_space = impl::color_space_lib_to_pub(cinfo.out_color_space);
  return JPEGImageInfo{width, height, static_cast<std::uint16_t>(cinfo.out_color_components), out_color_space};
}

bool JPEGDecompressionCycle::decompress(RowPointers& row_pointers)
{
  auto& cinfo = obj_.impl_->cinfo;

  const auto region_valid = !region_.empty();
  const auto skip_lines_top = region_valid ? region_.y0() : 0;
  const auto skip_lines_bottom = region_valid ? cinfo.output_height - region_.y_end() : 0;

  if (setjmp(obj_.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
  jpeg_skip_scanlines(&cinfo, static_cast<JDIMENSION>(skip_lines_top));
#endif

  while (cinfo.output_scanline < cinfo.output_height - skip_lines_bottom)
  {
    jpeg_read_scanlines(&cinfo, &row_pointers[cinfo.output_scanline - skip_lines_top], 1);
  }

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
  jpeg_skip_scanlines(&cinfo, static_cast<JDIMENSION>(skip_lines_bottom));
#endif

  jpeg_finish_decompress(&obj_.impl_->cinfo);
  finished_or_aborted_ = true;
  return true;

failure_state:
  jpeg_abort_decompress(&cinfo);
  finished_or_aborted_ = true;
  return false;
}


// -------------------------------
// Decompression related functions

void set_source(JPEGDecompressionObject& obj, FileReader& source)
{
  obj.reset_if_needed();

  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_stdio_src(&obj.impl_->cinfo, source.handle());

failure_state:;
}


void set_source(JPEGDecompressionObject& obj, MemoryReader& source)
{
  obj.reset_if_needed();

  // Unfortunately, the libjpeg API is not const-correct before version 9b (or before libjpeg-turbo 1.5.0).
  auto handle = const_cast<unsigned char*>(source.handle());

  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_mem_src(&obj.impl_->cinfo, handle, static_cast<unsigned long>(source.size()));

failure_state:;
}


JPEGImageInfo read_header(JPEGDecompressionObject& obj)
{
  obj.reset_if_needed();

  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_read_header(&obj.impl_->cinfo, TRUE);
  return obj.get_header_info();

failure_state:
  return JPEGImageInfo();  // invalid header info
}

}  // namespace impl

/// \endcond

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

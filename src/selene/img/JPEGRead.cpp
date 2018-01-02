// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/img/JPEGRead.hpp>
#include <selene/img/detail/JPEGDetail.hpp>

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
JPEGHeaderInfo::JPEGHeaderInfo(PixelIndex width_, PixelIndex height_, int nr_channels_, JPEGColorSpace color_space_)
    : width(width_), height(height_), nr_channels(nr_channels_), color_space(color_space_)
{
}

/** \brief Returns whether the contained JPEG header information is valid.
 *
 * @return True, if the header information is valid; false otherwise.
 */
bool JPEGHeaderInfo::is_valid() const
{
  return width > 0 && height > 0 && nr_channels > 0;
}

/// \cond INTERNAL

struct JPEGDecompressionObject::Impl
{
  jpeg_decompress_struct cinfo;
  detail::JPEGErrorManager error_manager;
  bool valid = false;
};

JPEGDecompressionObject::JPEGDecompressionObject() : impl_(std::make_unique<JPEGDecompressionObject::Impl>())
{
  impl_->cinfo.err = jpeg_std_error(&impl_->error_manager.pub);
  impl_->cinfo.err->error_exit = detail::error_exit;
  impl_->cinfo.err->output_message = detail::output_message;
  jpeg_create_decompress(&impl_->cinfo);
  impl_->valid = true;
}

JPEGDecompressionObject::~JPEGDecompressionObject()
{
  jpeg_destroy_decompress(&impl_->cinfo);
}

bool JPEGDecompressionObject::valid() const
{
  return impl_->valid;
}

JPEGHeaderInfo JPEGDecompressionObject::get_header_info() const
{
  const auto color_space = detail::color_space_lib_to_pub(impl_->cinfo.jpeg_color_space);
  return JPEGHeaderInfo(PixelIndex(impl_->cinfo.image_width), PixelIndex(impl_->cinfo.image_height),
                        impl_->cinfo.num_components, color_space);
}

void JPEGDecompressionObject::set_decompression_parameters(JPEGColorSpace out_color_space)
{
  if (out_color_space != JPEGColorSpace::Auto)
  {
    impl_->cinfo.out_color_space = detail::color_space_pub_to_lib(out_color_space);
  }
}

bool JPEGDecompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

const MessageLog& JPEGDecompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}


namespace detail {

JPEGDecompressionCycle::JPEGDecompressionCycle(JPEGDecompressionObject& obj, const BoundingBox& region)
    : obj_(obj), region_(region)
{
  auto& cinfo = obj_.impl_->cinfo;

  jpeg_start_decompress(&cinfo);

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
  if (!region_.empty() && region_.width() < cinfo.output_width)
  {
    // Enable partial decompression of each scanline
    JDIMENSION xoffset = region_.x0();
    JDIMENSION width = region_.width();
    jpeg_crop_scanline(&cinfo, &xoffset, &width);
    region_ = BoundingBox(PixelIndex(xoffset), region_.y0(), PixelIndex(width), region_.height());
  }
#endif
}

JPEGDecompressionCycle::~JPEGDecompressionCycle()
{
  jpeg_finish_decompress(&obj_.impl_->cinfo);
}

JPEGOutputInfo JPEGDecompressionCycle::get_output_info() const
{
  auto& cinfo = obj_.impl_->cinfo;
  SELENE_FORCED_ASSERT(cinfo.out_color_components == cinfo.output_components);
  const auto out_color_space = detail::color_space_lib_to_pub(cinfo.out_color_space);
  return JPEGOutputInfo(PixelIndex(cinfo.output_width), PixelIndex(cinfo.output_height), cinfo.out_color_components,
                        out_color_space);
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

  return true;

failure_state:
  jpeg_abort_decompress(&cinfo);
  return false;
}


// -------------------------------
// Decompression related functions

void set_source(JPEGDecompressionObject& obj, FileReader& source)
{
  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_stdio_src(&obj.impl_->cinfo, source.handle());

failure_state:;
}


void set_source(JPEGDecompressionObject& obj, MemoryReader& source)
{
  // Unfortunately, the libjpeg API is not const-correct before version 9b (or before libjpeg-turbo 1.5.0).
  auto handle = const_cast<unsigned char*>(source.handle());

  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_mem_src(&obj.impl_->cinfo, handle, static_cast<unsigned long>(source.size()));

failure_state:;
}


JPEGHeaderInfo read_header(JPEGDecompressionObject& obj)
{
  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_read_header(&obj.impl_->cinfo, TRUE);
  return obj.get_header_info();

failure_state:
  return JPEGHeaderInfo();  // invalid header info
}

}  // namespace detail

/// \endcond

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

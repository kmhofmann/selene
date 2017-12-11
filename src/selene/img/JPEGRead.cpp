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

struct JPEGOutputInfo
{
  const PixelIndex width;
  const PixelIndex height;
  const int nr_channels;
  const JPEGColorSpace color_space;

  JPEGOutputInfo(PixelIndex width_, PixelIndex height_, int nr_channels_, JPEGColorSpace color_space_)
      : width(width_), height(height_), nr_channels(nr_channels_), color_space(color_space_)
  {
  }
};


class JPEGDecompressionCycle
{
public:
  JPEGDecompressionCycle(JPEGDecompressionObject& obj, const BoundingBox& region);

  ~JPEGDecompressionCycle();

  JPEGOutputInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  JPEGDecompressionObject& obj_;
  BoundingBox region_;
};


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

// ----------------
// Public functions


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

  auto scope_exit = [&source, rewind, messages, &obj, src_pos]() {
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
  return read_jpeg(obj, source, options, messages, nullptr);
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
  const auto output_pixel_format = detail::color_space_to_pixel_format(output_info.color_space);
  const auto output_sample_format = SampleFormat::UnsignedInteger;

  ImageData img;
  img.allocate(output_width, output_height, output_nr_channels, output_nr_bytes_per_channel, output_pixel_format,
               output_sample_format);
  auto row_pointers = get_row_pointers(img);
  const auto dec_success = cycle.decompress(row_pointers);

  if (!dec_success)
  {
    img.clear();  // invalidates image data
  }

  detail::assign_message_log(obj, messages);
  return img;
}


// ----------
// Explicit instantiations:

template JPEGHeaderInfo read_jpeg_header<FileReader>(FileReader&, bool, MessageLog*);
template JPEGHeaderInfo read_jpeg_header<MemoryReader>(MemoryReader&, bool, MessageLog*);

template JPEGHeaderInfo read_jpeg_header<FileReader>(JPEGDecompressionObject&, FileReader&, bool, MessageLog*);
template JPEGHeaderInfo read_jpeg_header<MemoryReader>(JPEGDecompressionObject&, MemoryReader&, bool, MessageLog*);

template ImageData read_jpeg<FileReader>(FileReader&, JPEGDecompressionOptions, MessageLog*);
template ImageData read_jpeg<MemoryReader>(MemoryReader&, JPEGDecompressionOptions, MessageLog*);

template ImageData read_jpeg<FileReader>(JPEGDecompressionObject&, FileReader&, JPEGDecompressionOptions, MessageLog*,
                                         const JPEGHeaderInfo*);
template ImageData read_jpeg<MemoryReader>(JPEGDecompressionObject&, MemoryReader&, JPEGDecompressionOptions,
                                           MessageLog*, const JPEGHeaderInfo*);

/// \endcond

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/img/JPEGWrite.hpp>
#include <selene/img/detail/JPEGDetail.hpp>

#include <jpeglib.h>

#include <csetjmp>
#include <cstdio>
#include <stdexcept>

namespace selene {
namespace img {

/// \cond INTERNAL

struct JPEGCompressionObject::Impl
{
  jpeg_compress_struct cinfo;
  detail::JPEGErrorManager error_manager;

  unsigned char* output_buffer = nullptr;  // temporary buffer for in-memory decompression
  unsigned long output_size = 0;  // size of in-memory buffer

  bool valid = false;
};

JPEGCompressionObject::JPEGCompressionObject() : impl_(std::make_unique<JPEGCompressionObject::Impl>())
{
  impl_->cinfo.err = jpeg_std_error(&impl_->error_manager.pub);
  impl_->cinfo.err->error_exit = detail::error_exit;
  impl_->cinfo.err->output_message = detail::output_message;
  jpeg_create_compress(&impl_->cinfo);
  impl_->valid = true;
}

JPEGCompressionObject::~JPEGCompressionObject()
{
  jpeg_destroy_compress(&impl_->cinfo);

  if (impl_->output_buffer != nullptr)
  {
    std::free(impl_->output_buffer);
    impl_->output_buffer = nullptr;
  }
}

bool JPEGCompressionObject::valid() const
{
  return impl_->valid;
}

bool JPEGCompressionObject::set_image_info(int width, int height, int nr_channels, JPEGColorSpace in_color_space)
{
  if (setjmp(impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  SELENE_ASSERT(in_color_space != JPEGColorSpace::Auto);

  if (in_color_space == JPEGColorSpace::Unknown)
  {
    impl_->error_manager.message_log.add_message("Cannot determine JPEG color space from pixel format of image data");
    return false;
  }

  impl_->cinfo.image_width = static_cast<JDIMENSION>(width);
  impl_->cinfo.image_height = static_cast<JDIMENSION>(height);
  impl_->cinfo.input_components = nr_channels;
  impl_->cinfo.in_color_space = detail::color_space_pub_to_lib(in_color_space);

  jpeg_set_defaults(&impl_->cinfo);
  return true;

failure_state:
  return false;
}

bool JPEGCompressionObject::set_compression_parameters(int quality, JPEGColorSpace color_space, bool optimize_coding)
{
  const auto force_baseline = TRUE;
  quality = clamp(quality, 0, 100);

  if (setjmp(impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  // Set output color space if explicitly specified; otherwise, defaults will have been set by jpeg_set_defaults()
  if (color_space != JPEGColorSpace::Auto)
  {
    jpeg_set_colorspace(&impl_->cinfo, detail::color_space_pub_to_lib(color_space));
  }

  jpeg_set_quality(&impl_->cinfo, quality, force_baseline);

  impl_->cinfo.optimize_coding = (optimize_coding ? TRUE : FALSE);
  return true;

failure_state:
  return false;
}

bool JPEGCompressionObject::error_state() const
{
  return impl_->error_manager.error_state;
}

const MessageLog& JPEGCompressionObject::message_log() const
{
  return impl_->error_manager.message_log;
}


namespace detail {

class JPEGCompressionCycle
{
public:
  explicit JPEGCompressionCycle(JPEGCompressionObject& obj);
  ~JPEGCompressionCycle();

  void compress(const ConstRowPointers& row_pointers);

private:
  JPEGCompressionObject& obj_;
};

JPEGCompressionCycle::JPEGCompressionCycle(JPEGCompressionObject& obj) : obj_(obj)
{
  jpeg_start_compress(&obj_.impl_->cinfo, TRUE);
}

JPEGCompressionCycle::~JPEGCompressionCycle()
{
  jpeg_finish_compress(&obj_.impl_->cinfo);
}

void JPEGCompressionCycle::compress(const ConstRowPointers& row_pointers)
{
  auto& cinfo = obj_.impl_->cinfo;
  std::array<JSAMPLE*, 1> row_ptr = {{nullptr}};

  if (setjmp(obj_.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  while (cinfo.next_scanline < cinfo.image_height)
  {
    auto row_ptr_c = row_pointers[cinfo.next_scanline];
    // Hack to accommodate non-const correct API
    row_ptr[0] = const_cast<JSAMPLE*>(row_ptr_c);
    const auto nr_scanlines_written = jpeg_write_scanlines(&cinfo, row_ptr.data(), 1);
    SELENE_FORCED_ASSERT(nr_scanlines_written == 1);
  }

  return;

failure_state:
  jpeg_abort_compress(&cinfo);
}

// -----------------------------
// Compression related functions

void set_destination(JPEGCompressionObject& obj, io::FileWriter& sink)
{
  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_stdio_dest(&obj.impl_->cinfo, sink.handle());

failure_state:;
}

void set_destination(JPEGCompressionObject& obj, io::VectorWriter& /*sink*/)
{
  if (setjmp(obj.impl_->error_manager.setjmp_buffer))
  {
    goto failure_state;
  }

  jpeg_mem_dest(&obj.impl_->cinfo, &obj.impl_->output_buffer, &obj.impl_->output_size);

failure_state:;
}

bool flush_data_buffer(JPEGCompressionObject& /*obj*/, io::FileWriter& /*sink*/)
{
  return true;
}

bool flush_data_buffer(JPEGCompressionObject& obj, io::VectorWriter& sink)
{
  if (obj.impl_->output_buffer != nullptr)
  {
    const auto buf = static_cast<const std::uint8_t*>(obj.impl_->output_buffer);
    const auto len = static_cast<std::size_t>(obj.impl_->output_size);
    const auto nr_written = sink.write(buf, len);

    if (nr_written != static_cast<std::size_t>(obj.impl_->output_size))
    {
      return false;
    }
  }

  return true;
}

}  // namespace detail


// ----------------
// Public functions

template <typename SinkType>
bool write_jpeg(const ImageData& img_data, SinkType& sink, JPEGCompressionOptions options, MessageLog* messages)
{
  JPEGCompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return write_jpeg(img_data, obj, sink, options, messages);
};

template <typename SinkType>
bool write_jpeg(const ImageData& img_data, JPEGCompressionObject& obj, SinkType& sink, JPEGCompressionOptions options,
                MessageLog* messages)
{
  detail::set_destination(obj, sink);

  if (obj.error_state())
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  const auto nr_channels = img_data.nr_channels();
  const auto in_color_space = (options.in_color_space == JPEGColorSpace::Auto)
                                  ? detail::pixel_format_to_color_space(img_data.pixel_format())
                                  : options.in_color_space;

  const auto img_info_set = obj.set_image_info(static_cast<int>(img_data.width()), static_cast<int>(img_data.height()),
                                               static_cast<int>(nr_channels), in_color_space);

  if (!img_info_set)
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  const bool pars_set = obj.set_compression_parameters(options.quality, options.jpeg_color_space,
                                                       options.optimize_coding);

  if (!pars_set)
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  {
    detail::JPEGCompressionCycle cycle(obj);
    const auto row_pointers = get_row_pointers(img_data);
    cycle.compress(row_pointers);
    // Destructor of JPEGCompressionCycle calls jpeg_finish_compress(), which updates internal state
  }

  bool flushed = detail::flush_data_buffer(obj, sink);
  if (!flushed)
  {
    detail::assign_message_log(obj, messages);
    return false;
  }

  detail::assign_message_log(obj, messages);
  return !obj.error_state();
}

// ----------
// Explicit instantiations:

template bool write_jpeg<io::FileWriter>(const ImageData&, io::FileWriter&, JPEGCompressionOptions, MessageLog*);
template bool write_jpeg<io::VectorWriter>(const ImageData&, io::VectorWriter&, JPEGCompressionOptions, MessageLog*);

template bool write_jpeg<io::FileWriter>(const ImageData&, JPEGCompressionObject&, io::FileWriter&,
                                         JPEGCompressionOptions, MessageLog*);
template bool write_jpeg<io::VectorWriter>(const ImageData&, JPEGCompressionObject&, io::VectorWriter&,
                                           JPEGCompressionOptions, MessageLog*);

/// \endcond

}  // namespace img
}  // namespace selene

#endif  // defined(SELENE_WITH_LIBJPEG)

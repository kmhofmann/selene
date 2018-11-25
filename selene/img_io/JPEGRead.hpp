// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_READ_HPP
#define SELENE_IMG_JPEG_READ_HPP

/// @file

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/MemoryReader.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img_io/JPEGCommon.hpp>
#include <selene/img_io/impl/JPEGCommon.hpp>
#include <selene/img_io/impl/Util.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace sln {

struct JPEGImageInfo;
class JPEGDecompressionObject;

namespace impl {
class JPEGDecompressionCycle;
void set_source(JPEGDecompressionObject&, FileReader&);
void set_source(JPEGDecompressionObject&, MemoryReader&);
JPEGImageInfo read_header(JPEGDecompressionObject&);
}  // namespace impl

/** \brief JPEG image information, containing the image size, the number of channels, and the color space.
 *
 */
struct JPEGImageInfo
{
  const PixelLength width;  ///< Image width.
  const PixelLength height;  ///< Image height.
  const std::uint16_t nr_channels;  ///< Number of image channels.
  const JPEGColorSpace color_space;  ///< Image data color space.

  explicit JPEGImageInfo(PixelLength width_ = 0_px,
                         PixelLength height_ = 0_px,
                         std::uint16_t nr_channels_ = 0,
                         JPEGColorSpace color_space_ = JPEGColorSpace::Unknown);

  bool is_valid() const;
  std::uint16_t nr_bytes_per_channel() const { return 1; }

  std::size_t required_bytes() const { return (width * nr_channels) * height; }
};

/** \brief JPEG decompression options.
 *
 */
struct JPEGDecompressionOptions
{
  JPEGColorSpace out_color_space;  ///< The color space for the uncompressed data.
  BoundingBox region;  ///< If set (and supported), decompress only the specified image region (libjpeg-turbo).

  /** \brief Constructor, setting the respective JPEG decompression options.
   *
   * @param out_color_space_ The color space for the uncompressed data.
   * @param region_ If set (and supported), decompress only the specified image region (libjpeg-turbo).
   */
  explicit JPEGDecompressionOptions(JPEGColorSpace out_color_space_ = JPEGColorSpace::Auto
#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
                                    ,
                                    const BoundingBox& region_ = BoundingBox()
#endif
                                        )
      : out_color_space(out_color_space_)
#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
      , region(region_)
#endif
  {
  }
};

/** \brief Opaque JPEG decompression object, holding internal state.
 *
 */
class JPEGDecompressionObject
{
public:
  /// \cond INTERNAL
  JPEGDecompressionObject();
  ~JPEGDecompressionObject();

  bool valid() const;
  bool error_state() const;
  MessageLog& message_log();
  const MessageLog& message_log() const;

  JPEGImageInfo get_header_info() const;
  void set_decompression_parameters(JPEGColorSpace out_color_space = JPEGColorSpace::Auto);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void reset_if_needed();

  friend class impl::JPEGDecompressionCycle;
  friend void impl::set_source(JPEGDecompressionObject&, FileReader&);
  friend void impl::set_source(JPEGDecompressionObject&, MemoryReader&);
  friend JPEGImageInfo impl::read_header(JPEGDecompressionObject&);
};


/** \brief Reads header of JPEG image data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the original position after reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A JPEG header info object.
 */
template <typename SourceType>
JPEGImageInfo read_jpeg_header(SourceType&& source, bool rewind = false, MessageLog* messages = nullptr);

/** \brief Reads header of JPEG image data stream.
 *
 * This function overload enables re-use of a JPEGDecompressionObject instance.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param obj A JPEGDecompressionObject instance.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the original position after reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A JPEG header info object.
 */
template <typename SourceType>
JPEGImageInfo read_jpeg_header(JPEGDecompressionObject& obj,
                                SourceType&& source,
                                bool rewind = false,
                                MessageLog* messages = nullptr);

/** \brief Reads contents of a JPEG image data stream.
 *
 * The source position must be set to the beginning of the JPEG stream, including header. In case img::read_jpeg_header
 * is called before, then it must be with `rewind == true`.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param options The decompression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return An `ImageData` instance. Reading the JPEG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
ImageData<> read_jpeg(SourceType&& source,
                      JPEGDecompressionOptions options = JPEGDecompressionOptions(),
                      MessageLog* messages = nullptr);

/** \brief Reads contents of a JPEG image data stream.
 *
 * In case header information is not explicitly provided via the parameter `provided_header_info`, the source position
 * must be set to the beginning of the JPEG stream, including header. Otherwise img::read_jpeg_header must be called
 * before, with `rewind == false`, and the header information passed by pointer.
 *
 * This function overload enables re-use of a JPEGDecompressionObject instance.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param obj A JPEGDecompressionObject instance.
 * @param source Input source instance.
 * @param options The decompression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param provided_header_info Optional JPEG header information, obtained through a call to img::read_jpeg_header.
 * @return An `ImageData` instance. Reading the JPEG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
ImageData<> read_jpeg(JPEGDecompressionObject& obj,
                      SourceType&& source,
                      JPEGDecompressionOptions options = JPEGDecompressionOptions(),
                      MessageLog* messages = nullptr,
                      const JPEGImageInfo* provided_header_info = nullptr);

/** Class with functionality to read header and data of a JPEG image data stream.
 *
 * Generally, the free functions read_jpeg() or read_jpeg_header() should be preferred, due to ease of use.
 *
 * read_jpeg(), however, does not allow reading of the decompressed image data into pre-allocated memory.
 * This is enabled by calling `get_output_image_info()` on an instance of this class, then allocating the respective
 * `ImageData<>` instance (which can be itself a view to pre-allocated memory), and finally calling
 * `read_image_data(ImageData<>&)`.
 *
 * A JPEGReader<> instance is stateful:
 * Calling of `read_header()`, `set_decompression_options()`, or `get_output_image_info()` is optional.
 * The only required function to be called in order to read the image data is `read_image_data()`.
 *
 * Multiple images can be read in sequence using the same JPEGReader<> (on the same thread).
 * The source may optionally be re-set using `set_source()`; this is required if the previous image has not been read
 * completely or successfully.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 */
template <typename SourceType>
class JPEGReader
{
public:
  JPEGReader();
  explicit JPEGReader(SourceType& source, JPEGDecompressionOptions options = JPEGDecompressionOptions());

  void set_source(SourceType& source);

  JPEGImageInfo read_header();
  void set_decompression_options(JPEGDecompressionOptions options);

  JPEGImageInfo get_output_image_info();
  ImageData<> read_image_data();
  bool read_image_data(ImageData<>& img_data);

  MessageLog& message_log();

private:
  SourceType* source_;
  JPEGDecompressionOptions options_;
  mutable JPEGDecompressionObject obj_;
  mutable std::unique_ptr<impl::JPEGDecompressionCycle> cycle_;
  bool header_read_ = false;
  bool valid_header_read_ = false;

  void reset();
};

// ----------
// Implementation:

namespace impl {

class JPEGDecompressionCycle
{
public:
  JPEGDecompressionCycle(JPEGDecompressionObject& obj, const BoundingBox& region);
  ~JPEGDecompressionCycle();

  JPEGImageInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  JPEGDecompressionObject& obj_;
  BoundingBox region_;
  bool finished_or_aborted_ = false;
};

}  // namespace impl


template <typename SourceType>
JPEGImageInfo read_jpeg_header(SourceType&& source, bool rewind, MessageLog* messages)
{
  JPEGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_jpeg_header(obj, std::forward<SourceType>(source), rewind, messages);
}

template <typename SourceType>
JPEGImageInfo read_jpeg_header(JPEGDecompressionObject& obj, SourceType&& source, bool rewind, MessageLog* messages)
{
  const auto src_pos = source.position();

  auto scope_exit = [&source, rewind, messages, &obj, src_pos]() {
    if (rewind)
    {
      source.seek_abs(src_pos);
    }

    impl::assign_message_log(obj, messages);
  };

  impl::set_source(obj, source);

  if (obj.error_state())
  {
    scope_exit();
    return JPEGImageInfo();
  }

  const auto header_info = impl::read_header(obj);
  scope_exit();
  return header_info;
}

template <typename SourceType>
ImageData<> read_jpeg(SourceType&& source, JPEGDecompressionOptions options, MessageLog* messages)
{
  JPEGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_jpeg(obj, std::forward<SourceType>(source), options, messages, nullptr);
}

template <typename SourceType>
ImageData<> read_jpeg(JPEGDecompressionObject& obj,
                      SourceType&& source,
                      JPEGDecompressionOptions options,
                      MessageLog* messages,
                      const JPEGImageInfo* provided_header_info)
{
  if (!provided_header_info)
  {
    impl::set_source(obj, source);

    if (obj.error_state())
    {
      impl::assign_message_log(obj, messages);
      return ImageData<>();
    }
  }

  const JPEGImageInfo header_info = provided_header_info ? *provided_header_info : impl::read_header(obj);

  if (!header_info.is_valid())
  {
    impl::assign_message_log(obj, messages);
    return ImageData<>();
  }

  obj.set_decompression_parameters(options.out_color_space);

  impl::JPEGDecompressionCycle cycle(obj, options.region);

  const auto output_info = cycle.get_output_info();
  const auto output_width = output_info.width;
  const auto output_height = output_info.height;
  const auto output_nr_channels = output_info.nr_channels;
  const auto output_nr_bytes_per_channel = 1;
  const auto output_stride_bytes = Stride{0};  // will be chosen s.t. image content is tightly packed
  const auto output_pixel_format = impl::color_space_to_pixel_format(output_info.color_space);
  const auto output_sample_format = SampleFormat::UnsignedInteger;

  ImageData<> img(output_width, output_height, output_nr_channels, output_nr_bytes_per_channel, output_stride_bytes,
                  output_pixel_format, output_sample_format);
  auto row_pointers = get_row_pointers(img);
  const auto dec_success = cycle.decompress(row_pointers);

  if (!dec_success)
  {
    img.clear();  // invalidates image data
  }

  impl::assign_message_log(obj, messages);
  return img;
}


template <typename SourceType>
JPEGReader<SourceType>::JPEGReader()
    : source_(nullptr), options_(JPEGDecompressionOptions())
{

}

template <typename SourceType>
JPEGReader<SourceType>::JPEGReader(SourceType& source, JPEGDecompressionOptions options)
    : source_(&source), options_(options)
{
  impl::set_source(obj_, source);
}

template <typename SourceType>
void JPEGReader<SourceType>::set_source(SourceType& source)
{
  reset();
  source_ = &source;
  impl::set_source(obj_, source);
}

template <typename SourceType>
JPEGImageInfo JPEGReader<SourceType>::read_header()
{
  if (source_ == nullptr)
  {
    return JPEGImageInfo();
  }

  if (cycle_)
  {
    throw std::runtime_error("JPEGReader: Cannot call read_header() after call to get_output_image_info() or read_image_data().");
  }

  const JPEGImageInfo header_info = impl::read_header(obj_);
  header_read_ = true;
  valid_header_read_ = header_info.is_valid();
  return header_info;
}

template <typename SourceType>
void JPEGReader<SourceType>::set_decompression_options(JPEGDecompressionOptions options)
{
  if (cycle_)
  {
    throw std::runtime_error("JPEGReader: Cannot call set_decompression_options() after call to get_output_image_info() or read_image_data().");
  }

  options_ = options;
}

template <typename SourceType>
JPEGImageInfo JPEGReader<SourceType>::get_output_image_info()
{
  if (!header_read_)
  {
    read_header();
  }

  if (!valid_header_read_)
  {
    return JPEGImageInfo();
  }

  if (!cycle_)
  {
    obj_.set_decompression_parameters(options_.out_color_space);
    cycle_ = std::make_unique<impl::JPEGDecompressionCycle>(obj_, options_.region);
  }

  return cycle_->get_output_info();
}

template <typename SourceType>
ImageData<> JPEGReader<SourceType>::read_image_data()
{
  ImageData<> img_data;
  read_image_data(img_data);
  return img_data;
}

/** \brief XXX
 *
 * @tparam SourceType
 * @param img_data
 * @return True, if reading the image data was successful; false otherwise.
 */
template <typename SourceType>
bool JPEGReader<SourceType>::read_image_data(ImageData<>& img_data)
{
  if (!header_read_)
  {
    read_header();
  }

  if (!valid_header_read_)
  {
    return false;
  }

  const auto output_info = get_output_image_info();

  if (!output_info.is_valid())
  {
    return false;
  }

  const auto output_width = output_info.width;
  const auto output_height = output_info.height;
  const auto output_nr_channels = output_info.nr_channels;
  const auto output_nr_bytes_per_channel = 1;
  const auto output_stride_bytes = Stride{0};  // will be chosen s.t. image content is tightly packed
  const auto output_pixel_format = impl::color_space_to_pixel_format(output_info.color_space);
  const auto output_sample_format = SampleFormat::UnsignedInteger;

  img_data.maybe_allocate(output_width, output_height, output_nr_channels, output_nr_bytes_per_channel,
                          output_stride_bytes, output_pixel_format, output_sample_format);
  auto row_pointers = get_row_pointers(img_data);
  const auto dec_success = cycle_->decompress(row_pointers);

  reset();

  if (!dec_success)
  {
    return false;
  }

  return true;
}

template <typename SourceType>
MessageLog& JPEGReader<SourceType>::message_log()
{
  return obj_.message_log();
}

template <typename SourceType>
void JPEGReader<SourceType>::reset()
{
  // reset internal state
  cycle_ = nullptr;
  header_read_ = false;
  valid_header_read_ = false;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_JPEG_READ_HPP

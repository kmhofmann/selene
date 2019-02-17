// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_READ_HPP
#define SELENE_IMG_PNG_READ_HPP

/// @file

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/MemoryReader.hpp>

#include <selene/img/common/BoundingBox.hpp>
#include <selene/img/common/PixelFormat.hpp>
#include <selene/img/common/RowPointers.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/_impl/Utils.hpp>
#include <selene/img/dynamic/_impl/RuntimeChecks.hpp>
#include <selene/img/dynamic/_impl/StaticChecks.hpp>

#include <selene/img_io/_impl/Util.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace sln {

/// \addtogroup group-img-io-png
/// @{

class PNGImageInfo;
struct PNGDecompressionOptions;
class PNGDecompressionObject;

namespace impl {
class PNGDecompressionCycle;
void set_source(PNGDecompressionObject&, FileReader&);
void set_source(PNGDecompressionObject&, MemoryReader&);
PNGImageInfo read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
PNGImageInfo read_header(FileReader&, PNGDecompressionObject&);
PNGImageInfo read_header(MemoryReader&, PNGDecompressionObject&);
}  // namespace impl

/** \brief PNG image information, containing the image size, the number of channels, and the bit depth.
 *
 */
class PNGImageInfo
{
public:
  const PixelLength width;  ///< Image width.
  const PixelLength height;  ///< Image height.
  const std::int16_t nr_channels;  ///< Number of image channels.
  const std::int16_t bit_depth;  ///< Image bit depth (8 or 16).

  explicit PNGImageInfo(PixelLength width_ = 0_px,
                        PixelLength height_ = 0_px,
                        std::int16_t nr_channels_ = 0,
                        std::int16_t bit_depth_ = 0);

  bool is_valid() const;
  std::int16_t nr_bytes_per_channel() const { return int16_t(bit_depth / 8); }

  std::size_t required_bytes() const { return std::size_t((width * nr_channels * nr_bytes_per_channel()) * height); }
};

/** \brief PNG decompression options.
 *
 * For more detailed information, consult the libpng manual (libpng-manual.txt) provided with every libpng source
 * distribution, or available here: http://www.libpng.org/pub/png/libpng-manual.txt
 */
struct PNGDecompressionOptions
{
  bool force_bit_depth_8;  ///< Enforce a bit depth of 8 for 16-bit inputs.
  bool set_background;  ///< Composite against supplied background color, removing alpha channel (RGBA -> RGB, YA -> Y).
  bool strip_alpha_channel;  ///< Remove a potential alpha channel (RGBA -> RGB, YA -> Y).
  bool swap_alpha_channel;  ///< Read ARGB instead of RGBA, for RGBA images.
  bool set_bgr;  ///< Convert RGB to BGR.
  bool invert_alpha_channel;  ///< Invert values in alpha channel (e.g. 0 -> 255).
  bool invert_monochrome;  ///< Invert grayscale or grayscale_alpha image values.
  bool convert_gray_to_rgb;  ///< Convert grayscale images to RGB.
  bool convert_rgb_to_gray;  ///< Convert RGB images to grayscale.
  bool keep_big_endian;  ///< Keep big endian representation. (Defaults to false; data will be converted to little endian.)

  /** \brief Constructor. Sets the respective PNG decompression options.
   *
   * @param force_bit_depth_8_ Enforce a bit depth of 8 for 16-bit inputs.
   * @param set_background_ Composite against supplied background color, removing alpha channel (RGBA -> RGB, YA -> Y).
   * @param strip_alpha_channel_ Remove a potential alpha channel (RGBA -> RGB, YA -> Y).
   * @param swap_alpha_channel_ Read ARGB instead of RGBA, for RGBA images.
   * @param set_bgr_ Convert RGB to BGR.
   * @param invert_alpha_channel_ Invert values in alpha channel (e.g. 0 -> 255).
   * @param invert_monochrome_ Invert grayscale or grayscale_alpha image values.
   * @param convert_gray_to_rgb_ Convert grayscale images to RGB.
   * @param convert_rgb_to_gray_ Convert RGB images to grayscale.
   */
  explicit PNGDecompressionOptions(bool force_bit_depth_8_ = false,
                                   bool set_background_ = false,
                                   bool strip_alpha_channel_ = false,
                                   bool swap_alpha_channel_ = false,
                                   bool set_bgr_ = false,
                                   bool invert_alpha_channel_ = false,
                                   bool invert_monochrome_ = false,
                                   bool convert_gray_to_rgb_ = false,
                                   bool convert_rgb_to_gray_ = false,
                                   bool keep_big_endian_ = false)
      : force_bit_depth_8(force_bit_depth_8_)
      , set_background(set_background_)
      , strip_alpha_channel(strip_alpha_channel_)
      , swap_alpha_channel(swap_alpha_channel_)
      , set_bgr(set_bgr_)
      , invert_alpha_channel(invert_alpha_channel_)
      , invert_monochrome(invert_monochrome_)
      , convert_gray_to_rgb(convert_gray_to_rgb_)
      , convert_rgb_to_gray(convert_rgb_to_gray_)
      , keep_big_endian(keep_big_endian_)
  {
  }
};

/** \brief Opaque PNG decompression object, holding internal state.
 */
class PNGDecompressionObject
{
public:
  /// \cond INTERNAL
  PNGDecompressionObject();
  ~PNGDecompressionObject();

  bool valid() const;
  bool error_state() const;
  MessageLog& message_log();
  const MessageLog& message_log() const;

  bool set_decompression_parameters(bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
  PixelFormat get_pixel_format() const;
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void allocate();
  void deallocate();
  void reset_if_needed();

  friend class impl::PNGDecompressionCycle;
  friend void impl::set_source(PNGDecompressionObject&, FileReader&);
  friend void impl::set_source(PNGDecompressionObject&, MemoryReader&);
  friend PNGImageInfo impl::read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
};

/** \brief Reads header of PNG image data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the original position after reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A PNG header info object.
 */
template <typename SourceType>
PNGImageInfo read_png_header(SourceType&& source, bool rewind = false, MessageLog* messages = nullptr);

/** \brief Reads header of PNG image data stream.
 *
 * This function overload enables re-use of a PNGDecompressionObject instance.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param obj A PNGDecompressionObject instance.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the original position after reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A PNG header info object.
 */
template <typename SourceType>
PNGImageInfo read_png_header(PNGDecompressionObject& obj,
                              SourceType&& source,
                              bool rewind = false,
                              MessageLog* messages = nullptr);

/** \brief Reads contents of a PNG image data stream.
 *
 * The source position must be set to the beginning of the PNG stream, including header. In case img::read_png_header
 * is called before, then it must be with `rewind == true`.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param options The decompression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A `DynImage` instance. Reading the PNG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename Allocator = default_bytes_allocator, typename SourceType>
DynImage<Allocator> read_png(SourceType&& source,
                             PNGDecompressionOptions options = PNGDecompressionOptions(),
                             MessageLog* messages = nullptr);

/** \brief Reads contents of a PNG image data stream.
 *
 * In case header information is not explicitly provided via the parameter `provided_header_info`, the source position
 * must be set to the beginning of the PNG stream, including header. Otherwise img::read_png_header must be called
 * before, with `rewind == false`, and the header information passed by pointer.
 *
 * This function overload enables re-use of a PNGDecompressionObject instance.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param obj A PNGDecompressionObject instance.
 * @param source Input source instance.
 * @param options The decompression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param provided_header_info Optional PNG header information, obtained through a call to img::read_png_header.
 * @return A `DynImage` instance. Reading the PNG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename Allocator = default_bytes_allocator, typename SourceType>
DynImage<Allocator> read_png(PNGDecompressionObject& obj,
                             SourceType&& source,
                             PNGDecompressionOptions options = PNGDecompressionOptions(),
                             MessageLog* messages = nullptr,
                             const PNGImageInfo* provided_header_info = nullptr);

/** Class with functionality to read header and data of a PNG image data stream.
 *
 * Generally, the free functions read_png() or read_png_header() should be preferred, due to ease of use.
 *
 * read_png(), however, does not allow reading of the decompressed image data into pre-allocated memory.
 * This is enabled by calling `get_output_image_info()` on an instance of this class, then allocating the respective
 * `DynImage` instance (or by providing a `DynImageView` into pre-allocated memory), and finally calling
 * `read_image_data(DynImage&)` or `read_image_data(MutableDynImageView&)`.
 *
 * A PNGReader<> instance is stateful:
 * Calling of `read_header()`, `set_decompression_options()`, or `get_output_image_info()` is optional.
 * The only required function to be called in order to read the image data is `read_image_data()`.
 *
 * Multiple images can be read in sequence using the same PNGReader<> (on the same thread).
 * The source may optionally be re-set using `set_source()`; this is required if the previous image has not been read
 * completely or successfully.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 */
template <typename SourceType>
class PNGReader
{
public:
  PNGReader();
  explicit PNGReader(SourceType& source, PNGDecompressionOptions options = PNGDecompressionOptions());

  void set_source(SourceType& source);

  PNGImageInfo read_header();
  void set_decompression_options(PNGDecompressionOptions options);

  PNGImageInfo get_output_image_info();
  template <typename Allocator = default_bytes_allocator> DynImage<Allocator> read_image_data();
  template <typename DynImageOrView> bool read_image_data(DynImageOrView& dyn_img_or_view);

  MessageLog& message_log();

private:
  SourceType* source_;
  PNGDecompressionOptions options_;
  mutable PNGDecompressionObject obj_;
  mutable std::unique_ptr<impl::PNGDecompressionCycle> cycle_;
  bool header_read_ = false;
  bool valid_header_read_ = false;

  void reset();
};

/// @}

// ----------
// Implementation:

namespace impl {

class PNGDecompressionCycle
{
public:
  explicit PNGDecompressionCycle(PNGDecompressionObject& obj);
  ~PNGDecompressionCycle();

  bool error_state() const;
  PNGImageInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  PNGDecompressionObject& obj_;
  bool error_state_;
};

}  // namespace impl


template <typename SourceType>
PNGImageInfo read_png_header(SourceType&& source, bool rewind, MessageLog* messages)
{
  PNGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_png_header(obj, std::forward<SourceType>(source), rewind, messages);
}

template <typename SourceType>
PNGImageInfo read_png_header(PNGDecompressionObject& obj, SourceType&& source, bool rewind, MessageLog* messages)
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
    return PNGImageInfo();
  }

  const auto header_info = impl::read_header(source, obj);
  scope_exit();
  return header_info;
}

template <typename Allocator, typename SourceType>
DynImage<Allocator> read_png(SourceType&& source, PNGDecompressionOptions options, MessageLog* messages)
{
  PNGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_png(obj, std::forward<SourceType>(source), options, messages);
}

template <typename Allocator, typename SourceType>
DynImage<Allocator> read_png(PNGDecompressionObject& obj,
                             SourceType&& source,
                             PNGDecompressionOptions options,
                             MessageLog* messages,
                             const PNGImageInfo* provided_header_info)
{
  if (!provided_header_info)
  {
    impl::set_source(obj, source);

    if (obj.error_state())
    {
      impl::assign_message_log(obj, messages);
      return DynImage<Allocator>();
    }
  }

  const PNGImageInfo header_info = provided_header_info ? *provided_header_info : impl::read_header(source, obj);

  if (!header_info.is_valid())
  {
    impl::assign_message_log(obj, messages);
    return DynImage<Allocator>();
  }

  const bool pars_set = obj.set_decompression_parameters(
      options.force_bit_depth_8, options.set_background, options.strip_alpha_channel, options.swap_alpha_channel,
      options.set_bgr, options.invert_alpha_channel, options.invert_monochrome, options.convert_gray_to_rgb,
      options.convert_rgb_to_gray, options.keep_big_endian);

  if (!pars_set)
  {
    impl::assign_message_log(obj, messages);
    return DynImage<Allocator>();
  }

  impl::PNGDecompressionCycle cycle(obj);

  if (cycle.error_state())
  {
    impl::assign_message_log(obj, messages);
    return DynImage<Allocator>();
  }

  const auto output_info = cycle.get_output_info();
  const auto output_width = output_info.width;
  const auto output_height = output_info.height;
  const auto output_nr_channels = output_info.nr_channels;
  const auto output_bit_depth = output_info.bit_depth;
  const auto output_nr_bytes_per_channel = output_bit_depth >> 3;
  const auto output_stride_bytes = Stride{0};  // will be chosen s.t. image content is tightly packed
  const auto output_pixel_format = obj.get_pixel_format();
  const auto output_sample_format = SampleFormat::UnsignedInteger;

  DynImage<Allocator> dyn_img({output_width, output_height, static_cast<std::int16_t>(output_nr_channels),
                               static_cast<std::uint8_t>(output_nr_bytes_per_channel), output_stride_bytes},
                              {output_pixel_format, output_sample_format});
  auto row_pointers = get_row_pointers(dyn_img);
  const auto dec_success = cycle.decompress(row_pointers);

  if (!dec_success)
  {
    dyn_img.clear();  // invalidates image data
  }

  impl::assign_message_log(obj, messages);
  return dyn_img;
}



template <typename SourceType>
PNGReader<SourceType>::PNGReader()
    : source_(nullptr), options_(PNGDecompressionOptions())
{
}

template <typename SourceType>
PNGReader<SourceType>::PNGReader(SourceType& source, PNGDecompressionOptions options)
    : source_(&source), options_(options)
{
  impl::set_source(obj_, source);
}

template <typename SourceType>
void PNGReader<SourceType>::set_source(SourceType& source)
{
  reset();
  source_ = &source;
  impl::set_source(obj_, source);
}

template <typename SourceType>
PNGImageInfo PNGReader<SourceType>::read_header()
{
  if (source_ == nullptr)
  {
    return PNGImageInfo();
  }

  if (cycle_)
  {
    throw std::runtime_error("PNGReader: Cannot call read_header() after call to get_output_image_info() or read_image_data().");
  }

  const PNGImageInfo header_info = impl::read_header(*source_, obj_);
  header_read_ = true;
  valid_header_read_ = header_info.is_valid();
  return header_info;
}

template <typename SourceType>
void PNGReader<SourceType>::set_decompression_options(PNGDecompressionOptions options)
{
  if (cycle_)
  {
    throw std::runtime_error("PNGReader: Cannot call set_decompression_options() after call to get_output_image_info() or read_image_data().");
  }

  options_ = options;
}

template <typename SourceType>
PNGImageInfo PNGReader<SourceType>::get_output_image_info()
{
  if (!header_read_)
  {
    read_header();
  }

  if (!valid_header_read_)
  {
    return PNGImageInfo();
  }

  if (!cycle_)
  {
    const bool pars_set = obj_.set_decompression_parameters(
        options_.force_bit_depth_8, options_.set_background, options_.strip_alpha_channel, options_.swap_alpha_channel,
        options_.set_bgr, options_.invert_alpha_channel, options_.invert_monochrome, options_.convert_gray_to_rgb,
        options_.convert_rgb_to_gray, options_.keep_big_endian);

    if (!pars_set)
    {
      return PNGImageInfo();
    }

    cycle_ = std::make_unique<impl::PNGDecompressionCycle>(obj_);
  }

  return cycle_->get_output_info();
}

template <typename SourceType>
template <typename Allocator>
DynImage<Allocator> PNGReader<SourceType>::read_image_data()
{
  DynImage<Allocator> dyn_img;
  read_image_data(dyn_img);
  return dyn_img;
}

template <typename SourceType>
template <typename DynImageOrView>
bool PNGReader<SourceType>::read_image_data(DynImageOrView& dyn_img_or_view)
{
  impl::static_assert_is_dyn_image_or_mutable_view<DynImageOrView>();

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
  const auto output_nr_bytes_per_channel = output_info.nr_bytes_per_channel();
  const auto output_stride_bytes = Stride{0};  // will be chosen s.t. image content is tightly packed
  const auto output_pixel_format = obj_.get_pixel_format();
  const auto output_sample_format = SampleFormat::UnsignedInteger;

  const auto output_layout = UntypedLayout{output_width, output_height, output_nr_channels, output_nr_bytes_per_channel,
                                           output_stride_bytes};
  const auto output_semantics = UntypedImageSemantics{output_pixel_format, output_sample_format};

  const bool prepare_success = impl::prepare_image_or_view(dyn_img_or_view, output_layout, output_semantics);
  if (!prepare_success)
  {
    return false;
  }

  auto row_pointers = get_row_pointers(dyn_img_or_view);
  const auto dec_success = cycle_->decompress(row_pointers);

  reset();

  if (!dec_success)
  {
    return false;
  }

  return true;
}

template <typename SourceType>
MessageLog& PNGReader<SourceType>::message_log()
{
  return obj_.message_log();
}

template <typename SourceType>
void PNGReader<SourceType>::reset()
{
  // reset internal state
  cycle_ = nullptr;
  header_read_ = false;
  valid_header_read_ = false;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBPNG)

#endif  // SELENE_IMG_PNG_READ_HPP

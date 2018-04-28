// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_READ_HPP
#define SELENE_IMG_PNG_READ_HPP

/// @file

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/MemoryReader.hpp>

#include <array>
#include <csetjmp>
#include <cstdio>
#include <memory>

namespace sln {

class PNGImageInfo;
struct PNGDecompressionOptions;
class PNGDecompressionObject;

namespace detail {
class PNGDecompressionCycle;
void set_source(PNGDecompressionObject&, FileReader&);
void set_source(PNGDecompressionObject&, MemoryReader&);
PNGImageInfo read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
PNGImageInfo read_header(FileReader&, PNGDecompressionObject&);
PNGImageInfo read_header(MemoryReader&, PNGDecompressionObject&);
}  // namespace detail

/** \brief JPEG header information, containing the image size, the number of channels, and the bit depth.
 *
 */
class PNGImageInfo
{
public:
  const PixelLength width;  ///< Image width.
  const PixelLength height;  ///< Image height.
  const int nr_channels;  ///< Number of image channels.
  const int bit_depth;  ///< Image bit depth (8 or 16).

  explicit PNGImageInfo(PixelLength width_ = 0_px,
                         PixelLength height_ = 0_px,
                         int nr_channels_ = 0,
                         int bit_depth_ = 0);

  bool is_valid() const;
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
                                   bool convert_rgb_to_gray_ = false)
      : force_bit_depth_8(force_bit_depth_8_)
      , set_background(set_background_)
      , strip_alpha_channel(strip_alpha_channel_)
      , swap_alpha_channel(swap_alpha_channel_)
      , set_bgr(set_bgr_)
      , invert_alpha_channel(invert_alpha_channel_)
      , invert_monochrome(invert_monochrome_)
      , convert_gray_to_rgb(convert_gray_to_rgb_)
      , convert_rgb_to_gray(convert_rgb_to_gray_)
  {
  }
};

/** \brief Opaque PNG decompression object, holding internal state.
 *
 */
class PNGDecompressionObject
{
public:
  /// \cond INTERNAL
  PNGDecompressionObject();
  ~PNGDecompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  bool set_decompression_parameters(bool, bool, bool, bool, bool, bool, bool, bool, bool);
  PixelFormat get_pixel_format() const;
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend class detail::PNGDecompressionCycle;
  friend void detail::set_source(PNGDecompressionObject&, FileReader&);
  friend void detail::set_source(PNGDecompressionObject&, MemoryReader&);
  friend PNGImageInfo detail::read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
};

/** \brief Reads header of PNG image data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the position before reading the header.
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
 * @param rewind If true, the source position will be re-set to the position before reading the header.
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
 * @return An `ImageData` instance. Reading the PNG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
ImageData<> read_png(SourceType&& source,
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
 * @return An `ImageData` instance. Reading the PNG stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
ImageData<> read_png(PNGDecompressionObject& obj,
                     SourceType&& source,
                     PNGDecompressionOptions options = PNGDecompressionOptions(),
                     MessageLog* messages = nullptr,
                     const PNGImageInfo* provided_header_info = nullptr);

// ----------
// Implementation:

namespace detail {

class PNGDecompressionCycle
{
public:
  explicit PNGDecompressionCycle(PNGDecompressionObject& obj);
  ~PNGDecompressionCycle() = default;

  bool error_state() const;
  PNGImageInfo get_output_info() const;
  bool decompress(RowPointers& row_pointers);

private:
  PNGDecompressionObject& obj_;
  bool error_state_;
};

}  // namespace detail


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

    detail::assign_message_log(obj, messages);
  };

  detail::set_source(obj, source);

  if (obj.error_state())
  {
    scope_exit();
    return PNGImageInfo();
  }

  const auto header_info = detail::read_header(source, obj);
  scope_exit();
  return header_info;
}

template <typename SourceType>
ImageData<> read_png(SourceType&& source, PNGDecompressionOptions options, MessageLog* messages)
{
  PNGDecompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return read_png(obj, std::forward<SourceType>(source), options, messages);
}

template <typename SourceType>
ImageData<> read_png(PNGDecompressionObject& obj,
                     SourceType&& source,
                     PNGDecompressionOptions options,
                     MessageLog* messages,
                     const PNGImageInfo* provided_header_info)
{
  if (!provided_header_info)
  {
    detail::set_source(obj, source);

    if (obj.error_state())
    {
      detail::assign_message_log(obj, messages);
      return ImageData<>();
    }
  }

  const PNGImageInfo header_info = provided_header_info ? *provided_header_info : detail::read_header(source, obj);

  if (!header_info.is_valid())
  {
    detail::assign_message_log(obj, messages);
    return ImageData<>();
  }

  const bool pars_set = obj.set_decompression_parameters(
      options.force_bit_depth_8, options.set_background, options.strip_alpha_channel, options.swap_alpha_channel,
      options.set_bgr, options.invert_alpha_channel, options.invert_monochrome, options.convert_gray_to_rgb,
      options.convert_rgb_to_gray);

  if (!pars_set)
  {
    detail::assign_message_log(obj, messages);
    return ImageData<>();
  }

  detail::PNGDecompressionCycle cycle(obj);

  if (cycle.error_state())
  {
    detail::assign_message_log(obj, messages);
    return ImageData<>();
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

  ImageData<> img(output_width, output_height, static_cast<std::uint16_t>(output_nr_channels),
                  static_cast<std::uint8_t>(output_nr_bytes_per_channel), output_stride_bytes, output_pixel_format,
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

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBPNG)

#endif  // SELENE_IMG_PNG_READ_HPP

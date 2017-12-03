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

namespace selene {
namespace img {

class PNGHeaderInfo;
struct PNGDecompressionOptions;
class PNGDecompressionObject;

namespace detail
{
class PNGDecompressionCycle;
void set_source(PNGDecompressionObject&, io::FileReader&);
void set_source(PNGDecompressionObject&, io::MemoryReader&);
PNGHeaderInfo read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
PNGHeaderInfo read_header(io::FileReader&, PNGDecompressionObject&);
PNGHeaderInfo read_header(io::MemoryReader&, PNGDecompressionObject&);
} // namespace detail

/** \brief JPEG header information, containing the image size, the number of channels, and the bit depth.
 *
 */
class PNGHeaderInfo
{
public:
  const Length width;  ///< Image width.
  const Length height;  ///< Image height.
  const int nr_channels;  ///< Number of image channels.
  const int bit_depth;  ///< Image bit depth (8 or 16).

  explicit PNGHeaderInfo(Length width_ = 0, Length height_ = 0, int nr_channels_ = 0, int bit_depth_ = 0);

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
  explicit PNGDecompressionOptions(bool force_bit_depth_8_ = false, bool set_background_ = false,
                                   bool strip_alpha_channel_ = false, bool swap_alpha_channel_ = false,
                                   bool set_bgr_ = false, bool invert_alpha_channel_ = false,
                                   bool invert_monochrome_ = false, bool convert_gray_to_rgb_ = false,
                                   bool convert_rgb_to_gray_ = false)
      : force_bit_depth_8(force_bit_depth_8_), set_background(set_background_),
        strip_alpha_channel(strip_alpha_channel_), swap_alpha_channel(swap_alpha_channel_),
        set_bgr(set_bgr_), invert_alpha_channel(invert_alpha_channel_),
        invert_monochrome(invert_monochrome_), convert_gray_to_rgb(convert_gray_to_rgb_),
        convert_rgb_to_gray(convert_rgb_to_gray_)
  {
  }
};

/** Opaque PNG decompression object, holding internal state.
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
  friend void detail::set_source(PNGDecompressionObject&, io::FileReader&);
  friend void detail::set_source(PNGDecompressionObject&, io::MemoryReader&);
  friend PNGHeaderInfo detail::read_header_info(PNGDecompressionObject&, const std::array<std::uint8_t, 8>&, bool);
};

template <typename SourceType>
PNGHeaderInfo read_png_header(SourceType& source, bool rewind = false, MessageLog* messages = nullptr);

template <typename SourceType>
PNGHeaderInfo read_png_header(PNGDecompressionObject& obj, SourceType& source, bool rewind = false,
                              MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_png(SourceType& source, PNGDecompressionOptions options = PNGDecompressionOptions(),
                   MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_png(PNGDecompressionObject& obj, SourceType& source,
                   PNGDecompressionOptions options = PNGDecompressionOptions(), MessageLog* messages = nullptr,
                   const PNGHeaderInfo* provided_header_info = nullptr);

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

#endif // SELENE_IMG_PNG_READ_HPP

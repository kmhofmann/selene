// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_READ_HPP
#define SELENE_IMG_JPEG_READ_HPP

/// @file

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/JPEGCommon.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/JPEGCommon.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/MemoryReader.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace sln {

struct JPEGHeaderInfo;
class JPEGDecompressionObject;

namespace detail {
class JPEGDecompressionCycle;
void set_source(JPEGDecompressionObject&, FileReader&);
void set_source(JPEGDecompressionObject&, MemoryReader&);
JPEGHeaderInfo read_header(JPEGDecompressionObject&);
}  // namespace detail

/** \brief JPEG header information, containing the image size, the number of channels, and the color space.
 *
 */
struct JPEGHeaderInfo
{
  const PixelIndex width;  ///< Image width.
  const PixelIndex height;  ///< Image height.
  const int nr_channels;  ///< Number of image channels.
  const JPEGColorSpace color_space;  ///< Image data color space.

  explicit JPEGHeaderInfo(PixelIndex width_ = 0_px,
                          PixelIndex height_ = 0_px,
                          int nr_channels_ = 0,
                          JPEGColorSpace color_space_ = JPEGColorSpace::Unknown);

  bool is_valid() const;
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

/** Opaque JPEG decompression object, holding internal state.
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
  const MessageLog& message_log() const;

  JPEGHeaderInfo get_header_info() const;
  void set_decompression_parameters(JPEGColorSpace out_color_space = JPEGColorSpace::Auto);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend class detail::JPEGDecompressionCycle;
  friend void detail::set_source(JPEGDecompressionObject&, FileReader&);
  friend void detail::set_source(JPEGDecompressionObject&, MemoryReader&);
  friend JPEGHeaderInfo detail::read_header(JPEGDecompressionObject&);
};


/** \brief Reads header of JPEG image data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the position before reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A JPEG header info object.
 */
template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(SourceType& source, bool rewind = false, MessageLog* messages = nullptr);

/** \brief Reads header of JPEG image data stream.
 *
 * This function overload enables re-use of a JPEGDecompressionObject instance.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param obj A JPEGDecompressionObject instance.
 * @param source Input source instance.
 * @param rewind If true, the source position will be re-set to the position before reading the header.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A JPEG header info object.
 */
template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(JPEGDecompressionObject& obj,
                                SourceType& source,
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
ImageData read_jpeg(SourceType& source,
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
ImageData read_jpeg(JPEGDecompressionObject& obj,
                    SourceType& source,
                    JPEGDecompressionOptions options = JPEGDecompressionOptions(),
                    MessageLog* messages = nullptr,
                    const JPEGHeaderInfo* provided_header_info = nullptr);

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_JPEG_READ_HPP

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

namespace selene {
namespace img {

struct JPEGHeaderInfo;
class JPEGDecompressionObject;

namespace detail
{
class JPEGDecompressionCycle;
void set_source(JPEGDecompressionObject&, io::FileReader&);
void set_source(JPEGDecompressionObject&, io::MemoryReader&);
JPEGHeaderInfo read_header(JPEGDecompressionObject&);
} // namespace detail

/** \brief JPEG header information, containing the image size, the number of channels, and the color space.
 *
 */
struct JPEGHeaderInfo
{
  const Index width;  ///< Image width.
  const Index height;  ///< Image height.
  const int nr_channels;  ///< Number of image channels.
  const JPEGColorSpace color_space;  ///< Image data color space.

  explicit JPEGHeaderInfo(Index width_ = 0, Index height_ = 0, int nr_channels_ = 0,
                          JPEGColorSpace color_space_ = JPEGColorSpace::Unknown);

  bool is_valid() const;
};

/** \brief JPEG decompression options.
 *
 */
struct JPEGDecompressionOptions
{
  JPEGColorSpace out_color_space;  ///< The color space for the uncompressed data.
  BoundingBox<Index> region;  ///< If set (and supported), decompress only the specified image region (libjpeg-turbo).

  /** \brief Constructor, setting the respective JPEG decompression options.
   *
   * @param out_color_space_ The color space for the uncompressed data.
   * @param region_ If set (and supported), decompress only the specified image region (libjpeg-turbo).
   */
  explicit JPEGDecompressionOptions(JPEGColorSpace out_color_space_ = JPEGColorSpace::Auto
#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
                                    , const BoundingBox<Index>& region_ = BoundingBox<Index>()
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
  friend void detail::set_source(JPEGDecompressionObject&, io::FileReader&);
  friend void detail::set_source(JPEGDecompressionObject&, io::MemoryReader&);
  friend JPEGHeaderInfo detail::read_header(JPEGDecompressionObject&);
};


template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(SourceType& source, bool rewind = false, MessageLog* messages = nullptr);

template <typename SourceType>
JPEGHeaderInfo read_jpeg_header(JPEGDecompressionObject& obj, SourceType& source, bool rewind = false,
                                MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_jpeg(SourceType& source, JPEGDecompressionOptions options = JPEGDecompressionOptions(),
               MessageLog* messages = nullptr);

template <typename SourceType>
ImageData read_jpeg(JPEGDecompressionObject& obj, SourceType& source,
               JPEGDecompressionOptions options = JPEGDecompressionOptions(), MessageLog* messages = nullptr,
               const JPEGHeaderInfo* provided_header_info = nullptr);

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBJPEG)

#endif // SELENE_IMG_JPEG_READ_HPP

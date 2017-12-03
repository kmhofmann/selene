// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_WRITE_HPP
#define SELENE_IMG_JPEG_WRITE_HPP

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

#include <selene/io/FileWriter.hpp>
#include <selene/io/VectorWriter.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace selene {
namespace img {

class JPEGCompressionObject;

namespace detail
{
class JPEGCompressionCycle;
void set_destination(JPEGCompressionObject&, io::FileWriter&);
void set_destination(JPEGCompressionObject&, io::VectorWriter&);
bool flush_data_buffer(JPEGCompressionObject&, io::FileWriter&);
bool flush_data_buffer(JPEGCompressionObject&, io::VectorWriter&);
} // namespace detail

/** \brief JPEG compression options.
 *
 */
struct JPEGCompressionOptions
{
  const int quality;  ///< Compression quality. May take values from 1 (worst) to 100 (best).
  const JPEGColorSpace in_color_space;  ///< Color space of the incoming, to-be-compressed data.
  const JPEGColorSpace jpeg_color_space;  ///< Color space of the compressed data inside the JPEG stream.
  bool optimize_coding;  ///< If true, compute optimal Huffman coding tables for the image (more expensive computation).

  /** \brief Constructor, setting the respective JPEG compression options.
   *
   * @param quality_ Compression quality. May take values from 1 (worst) to 100 (best).
   * @param in_color_space_ Color space of the incoming, to-be-compressed data.
   * @param jpeg_color_space_ Color space of the compressed data inside the JPEG stream.
   * @param optimize_coding_ If true, compute optimal Huffman coding tables for the image (more expensive computation).
   */
  explicit JPEGCompressionOptions(int quality_ = 95,
                                  JPEGColorSpace in_color_space_ = JPEGColorSpace::Auto,
                                  JPEGColorSpace jpeg_color_space_ = JPEGColorSpace::Auto,
                                  bool optimize_coding_ = false)
      : quality(quality_), in_color_space(in_color_space_), jpeg_color_space(jpeg_color_space_),
        optimize_coding(optimize_coding_)
  {
  }
};

/** Opaque JPEG compression object, holding internal state.
 *
 */
class JPEGCompressionObject
{
public:
  /// \cond INTERNAL
  JPEGCompressionObject();
  ~JPEGCompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  bool set_image_info(int width, int height, int nr_channels, JPEGColorSpace in_color_space);
  bool set_compression_parameters(
      int quality, JPEGColorSpace color_space = JPEGColorSpace::Auto,
      bool optimize_coding = false);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr <Impl> impl_;

  friend class detail::JPEGCompressionCycle;
  friend void detail::set_destination(JPEGCompressionObject&, io::FileWriter&);
  friend void detail::set_destination(JPEGCompressionObject&, io::VectorWriter&);
  friend bool detail::flush_data_buffer(JPEGCompressionObject&, io::FileWriter&);
  friend bool detail::flush_data_buffer(JPEGCompressionObject&, io::VectorWriter&);
};


template <typename SinkType>
bool write_jpeg(const ImageData& img_data, SinkType& sink, JPEGCompressionOptions options = JPEGCompressionOptions(),
                MessageLog* messages = nullptr);

template <typename SinkType>
bool write_jpeg(const ImageData& img_data, JPEGCompressionObject& obj, SinkType& sink,
                JPEGCompressionOptions options = JPEGCompressionOptions(), MessageLog* messages = nullptr);

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBJPEG)

#endif // SELENE_IMG_JPEG_WRITE_HPP

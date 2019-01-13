// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_JPEG_WRITE_HPP
#define SELENE_IMG_JPEG_WRITE_HPP

/// @file

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/common/BoundingBox.hpp>
#include <selene/img/common/RowPointers.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>
#include <selene/img/dynamic/_impl/StaticChecks.hpp>

#include <selene/img_io/jpeg/Common.hpp>
#include <selene/img_io/jpeg/_impl/Common.hpp>
#include <selene/img_io/_impl/Util.hpp>

#include <array>
#include <cstdio>
#include <memory>

namespace sln {

class JPEGCompressionObject;

namespace impl {
class JPEGCompressionCycle;
void set_destination(JPEGCompressionObject&, FileWriter&);
void set_destination(JPEGCompressionObject&, VectorWriter&);
bool flush_data_buffer(JPEGCompressionObject&, FileWriter&);
bool flush_data_buffer(JPEGCompressionObject&, VectorWriter&);
}  // namespace impl

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
      : quality(quality_)
      , in_color_space(in_color_space_)
      , jpeg_color_space(jpeg_color_space_)
      , optimize_coding(optimize_coding_)
  {
  }
};

/** \brief Opaque JPEG compression object, holding internal state.
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
  MessageLog& message_log();
  const MessageLog& message_log() const;

  bool set_image_info(int width, int height, int nr_channels, int nr_bytes_per_channel, JPEGColorSpace in_color_space);
  bool set_compression_parameters(int quality,
                                  JPEGColorSpace color_space = JPEGColorSpace::Auto,
                                  bool optimize_coding = false);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void reset_if_needed();

  friend class impl::JPEGCompressionCycle;
  friend void impl::set_destination(JPEGCompressionObject&, FileWriter&);
  friend void impl::set_destination(JPEGCompressionObject&, VectorWriter&);
  friend bool impl::flush_data_buffer(JPEGCompressionObject&, FileWriter&);
  friend bool impl::flush_data_buffer(JPEGCompressionObject&, VectorWriter&);
};


/** \brief Writes a JPEG image data stream, given the supplied uncompressed image data.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param img_data The image data to be written.
 * @param sink Output sink instance.
 * @param options The compression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename DynImageOrView, typename SinkType>
bool write_jpeg(const DynImageOrView& dyn_img_or_view,
                SinkType&& sink,
                JPEGCompressionOptions options = JPEGCompressionOptions(),
                MessageLog* messages = nullptr);

/** \brief Writes a JPEG image data stream, given the supplied uncompressed image data.
 *
 * This function overload enables re-use of a JPEGCompressionObject instance.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param img_data The image data to be written.
 * @param obj A JPEGCompressionObject instance.
 * @param sink Output sink instance.
 * @param options The compression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename DynImageOrView, typename SinkType>
bool write_jpeg(const DynImageOrView& dyn_img_or_view,
                JPEGCompressionObject& obj,
                SinkType&& sink,
                JPEGCompressionOptions options = JPEGCompressionOptions(),
                MessageLog* messages = nullptr);

// ----------
// Implementation:

namespace impl {

class JPEGCompressionCycle
{
public:
  explicit JPEGCompressionCycle(JPEGCompressionObject& obj);
  ~JPEGCompressionCycle();

  void compress(const ConstRowPointers& row_pointers);

private:
  JPEGCompressionObject& obj_;
};

}  // namespace impl


template <typename DynImageOrView, typename SinkType>
bool write_jpeg(const DynImageOrView& dyn_img_or_view,
                SinkType&& sink,
                JPEGCompressionOptions options,
                MessageLog* messages)
{
  JPEGCompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return write_jpeg(dyn_img_or_view, obj, std::forward<SinkType>(sink), options, messages);
}

template <typename DynImageOrView, typename SinkType>
bool write_jpeg(const DynImageOrView& dyn_img_or_view,
                JPEGCompressionObject& obj,
                SinkType&& sink,
                JPEGCompressionOptions options,
                MessageLog* messages)
{
  impl::static_check_is_dyn_image_or_view<DynImageOrView>();

  impl::set_destination(obj, sink);

  if (obj.error_state())
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  const auto nr_channels = dyn_img_or_view.nr_channels();
  const auto nr_bytes_per_channel = dyn_img_or_view.nr_bytes_per_channel();
  const auto in_color_space = (options.in_color_space == JPEGColorSpace::Auto)
                                  ? impl::pixel_format_to_color_space(dyn_img_or_view.pixel_format())
                                  : options.in_color_space;

  const auto img_info_set = obj.set_image_info(static_cast<int>(dyn_img_or_view.width()),
                                               static_cast<int>(dyn_img_or_view.height()),
                                               static_cast<int>(nr_channels),
                                               static_cast<int>(nr_bytes_per_channel),
                                               in_color_space);

  if (!img_info_set)
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  const bool pars_set = obj.set_compression_parameters(options.quality, options.jpeg_color_space,
                                                       options.optimize_coding);

  if (!pars_set)
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  {
    impl::JPEGCompressionCycle cycle(obj);
    const auto row_pointers = get_const_row_pointers(dyn_img_or_view);
    cycle.compress(row_pointers);
    // Destructor of JPEGCompressionCycle calls jpeg_finish_compress(), which updates internal state
  }

  bool flushed = impl::flush_data_buffer(obj, sink);
  if (!flushed)
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  impl::assign_message_log(obj, messages);
  return !obj.error_state();
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_JPEG_WRITE_HPP

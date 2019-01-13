// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_WRITE_HPP
#define SELENE_IMG_PNG_WRITE_HPP

/// @file

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/common/BoundingBox.hpp>
#include <selene/img/common/RowPointers.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/_impl/StaticChecks.hpp>

#include <selene/img_io/_impl/Util.hpp>

#include <array>
#include <csetjmp>
#include <cstdio>
#include <memory>

namespace sln {

// Forward declarations

struct PNGCompressionOptions;
class PNGCompressionObject;

namespace impl {
class PNGCompressionCycle;
void set_destination(PNGCompressionObject&, FileWriter&);
void set_destination(PNGCompressionObject&, VectorWriter&);
}  // namespace impl

/** \brief PNG compression options.
 *
 * For more detailed information, consult the libpng manual (libpng-manual.txt) provided with every libpng source
 * distribution, or available here: http://www.libpng.org/pub/png/libpng-manual.txt
 */
struct PNGCompressionOptions
{
  int compression_level;  ///< Compression level; may take values from 0 (no compression) to 9 ("maximal" compression).
  bool interlaced;  ///< If true, write PNG image as interlaced.
  bool set_bgr;  ///< If true, convert BGR (supplied) to RGB (written).
  bool invert_alpha_channel;  ///< If true, invert values in alpha channel (e.g. 0 -> 255).
  bool invert_monochrome;  ///< If true, invert grayscale or grayscale_alpha image values.

  /** \brief Constructor, setting the respective JPEG compression options.
   *
   * @param compression_level_ Compression level; may take values from 0 (no compression) to 9 ("maximal" compression).
   * Defaults to value 6.
   * @param interlaced_ If true, write PNG image as interlaced.
   * @param set_bgr_ If true, convert BGR (supplied) to RGB (written).
   * @param invert_alpha_channel_ If true, invert values in alpha channel (e.g. 0 -> 255).
   * @param invert_monochrome_ If true, invert grayscale or grayscale_alpha image values.
   */
  explicit PNGCompressionOptions(int compression_level_ = 6,
                                 bool interlaced_ = false,
                                 bool set_bgr_ = false,
                                 bool invert_alpha_channel_ = false,
                                 bool invert_monochrome_ = false)
      : compression_level(compression_level_)
      , interlaced(interlaced_)
      , set_bgr(set_bgr_)
      , invert_alpha_channel(invert_alpha_channel_)
      , invert_monochrome(invert_monochrome_)
  {
  }
};

/** \brief Opaque PNG compression object, holding internal state.
 */
class PNGCompressionObject
{
public:
  /// \cond INTERNAL
  PNGCompressionObject();
  ~PNGCompressionObject();

  bool valid() const;
  bool error_state() const;
  MessageLog& message_log();
  const MessageLog& message_log() const;

  bool set_image_info(int width, int height, int nr_channels, int bit_depth, bool interlaced, PixelFormat pixel_format);
  bool set_compression_parameters(int, bool);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void reset_if_needed();

  friend class impl::PNGCompressionCycle;
  friend void impl::set_destination(PNGCompressionObject&, FileWriter&);
  friend void impl::set_destination(PNGCompressionObject&, VectorWriter&);
};


/** \brief Writes a PNG image data stream, given the supplied uncompressed image data.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param img_data The image data to be written.
 * @param sink Output sink instance.
 * @param options The compression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename DynImageOrView, typename SinkType>
bool write_png(const DynImageOrView& dyn_img_or_view,
               SinkType&& sink,
               PNGCompressionOptions options = PNGCompressionOptions(),
               MessageLog* messages = nullptr);

/** \brief Writes a PNG image data stream, given the supplied uncompressed image data.
 *
 * This function overload enables re-use of a PNGCompressionObject instance.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param img_data The image data to be written.
 * @param obj A PNGCompressionObject instance.
 * @param sink Output sink instance.
 * @param options The compression options.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename DynImageOrView, typename SinkType>
bool write_png(const DynImageOrView& dyn_img_or_view,
               PNGCompressionObject& obj,
               SinkType&& sink,
               PNGCompressionOptions options = PNGCompressionOptions(),
               MessageLog* messages = nullptr);

// ----------
// Implementation:

namespace impl {

class PNGCompressionCycle
{
public:
  explicit PNGCompressionCycle(PNGCompressionObject& obj, bool, bool);
  ~PNGCompressionCycle();

  bool error_state() const;
  void compress(const ConstRowPointers& row_pointers);

private:
  PNGCompressionObject& obj_;
  bool error_state_;
};

}  // namespace impl


template <typename DynImageOrView, typename SinkType>
bool write_png(const DynImageOrView& dyn_img_or_view,
               SinkType&& sink,
               PNGCompressionOptions options,
               MessageLog* messages)
{
  PNGCompressionObject obj;
  SELENE_ASSERT(obj.valid());
  return write_png(dyn_img_or_view, obj, std::forward<SinkType>(sink), options, messages);
}

template <typename DynImageOrView, typename SinkType>
bool write_png(const DynImageOrView& dyn_img_or_view,
               PNGCompressionObject& obj,
               SinkType&& sink,
               PNGCompressionOptions options,
               MessageLog* messages)
{
  impl::static_check_is_dyn_image_or_view<DynImageOrView>();

  if (dyn_img_or_view.nr_bytes_per_channel() != 1 && dyn_img_or_view.nr_bytes_per_channel() != 2)
  {
    throw std::runtime_error("Unsupported bit depth of image data for PNG output");
  }

  impl::set_destination(obj, sink);

  if (obj.error_state())
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  const auto nr_channels = dyn_img_or_view.nr_channels();
  const auto bit_depth = dyn_img_or_view.nr_bytes_per_channel() == 1 ? 8 : 16;

  const bool img_info_set = obj.set_image_info(static_cast<int>(dyn_img_or_view.width()),
                                               static_cast<int>(dyn_img_or_view.height()),
                                               static_cast<int>(nr_channels), static_cast<int>(bit_depth),
                                               options.interlaced,
                                               dyn_img_or_view.pixel_format());

  if (!img_info_set)
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  const bool pars_set = obj.set_compression_parameters(options.compression_level, options.invert_alpha_channel);

  if (!pars_set)
  {
    impl::assign_message_log(obj, messages);
    return false;
  }

  impl::PNGCompressionCycle cycle(obj, options.set_bgr, options.invert_monochrome);
  const auto row_pointers = get_const_row_pointers(dyn_img_or_view);
  cycle.compress(row_pointers);

  impl::assign_message_log(obj, messages);
  return !obj.error_state();
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBPNG)

#endif  // SELENE_IMG_PNG_WRITE_HPP

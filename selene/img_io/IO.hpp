// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_HPP
#define SELENE_IMG_IO_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img_io/JPEGRead.hpp>
#include <selene/img_io/JPEGWrite.hpp>
#include <selene/img_io/PNGRead.hpp>
#include <selene/img_io/PNGWrite.hpp>

#include <stdexcept>

namespace sln {

/** \brief Describes a supported image format for reading or writing.
 */
enum class ImageFormat
{
  JPEG,  ///< JPEG image format.
  PNG  ///< PNG image format.
};

template <typename SourceType>
DynImage read_image(SourceType&& source, MessageLog* messages = nullptr);

template <typename SinkType>
bool write_image(const DynImage& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* messages = nullptr,
                 int jpeg_quality = 95);

template <ImageModifiability modifiability, typename SinkType>
bool write_image(const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* messages = nullptr,
                 int jpeg_quality = 95);

// ----------
// Implementation:

namespace impl {

void add_messages(const MessageLog& messages_src, MessageLog* messages_dst);

}  // namespace impl

/** \brief Reads an image stream, trying all supported formats.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A `DynImage` instance. Reading the image stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
DynImage read_image(SourceType&& source,
                    [[maybe_unused]] MessageLog* messages)
{
  bool reading_attempted = false;
  const auto source_pos = source.position();
  DynImage dyn_img;

  // First, try to read as JPEG image:

#if defined(SELENE_WITH_LIBJPEG)
  {
    JPEGDecompressionObject obj;
    MessageLog messages_jpeg;
    auto header_info = read_jpeg_header(obj, std::forward<SourceType>(source), false, &messages_jpeg);
    reading_attempted = true;

    if (header_info.is_valid())
    {
      dyn_img = read_jpeg(obj, std::forward<SourceType>(source), JPEGDecompressionOptions(), &messages_jpeg,
                          &header_info);

      impl::add_messages(messages_jpeg, messages);
      return dyn_img;
    }
  }
#endif  // defined(SELENE_WITH_LIBJPEG)

  // In case that failed, try to read as PNG image:
  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);

#if defined(SELENE_WITH_LIBPNG)
  {
    PNGDecompressionObject obj;
    MessageLog messages_png;
    auto header_info = read_png_header(obj, std::forward<SourceType>(source), false, &messages_png);
    reading_attempted = true;

    if (header_info.is_valid())
    {
      dyn_img = read_png(obj, std::forward<SourceType>(source), PNGDecompressionOptions(), &messages_png,
                         &header_info);

      impl::add_messages(messages_png, messages);
      return dyn_img;
    }
  }
#endif  // defined(SELENE_WITH_LIBPNG)

  // Image reading failed: return an ImageData instance that is invalid:

  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);

  if (!reading_attempted)
  {
    throw std::runtime_error("ERROR: Image reading unsupported; recompile with the respective external dependencies.");
  }

  return dyn_img;
}

template <typename SinkType>
bool write_image(const DynImage& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* messages,
                 int jpeg_quality)
{
  return write_image(dyn_img.view(), format, sink, messages, jpeg_quality);
}

/** \brief Writes an image stream, given the supplied uncompressed image data.
 *
 * @tparam modifiability The dynamic image modifiability (mutable or constant).
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param dyn_img The dynamic image to be written.
 * @param format Desired output image format.
 * @param sink Output sink instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param jpeg_quality Compression quality for JPEG data. May take values from 1 (worst) to 100 (best). Only meaningful
 * in combination with `format` set to `ImageFormat::JPEG`, otherwise this value will be ignored.
 * @return True, if the write operation was successful; false otherwise.
 */
template <ImageModifiability modifiability, typename SinkType>
bool write_image([[maybe_unused]] const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 [[maybe_unused]] SinkType&& sink,
                 [[maybe_unused]] MessageLog* messages,
                 [[maybe_unused]] int jpeg_quality)
{
  if (format == ImageFormat::JPEG)
  {
#if defined(SELENE_WITH_LIBJPEG)
    MessageLog messages_jpeg;
    bool success = write_jpeg(dyn_img_view, std::forward<SinkType>(sink), JPEGCompressionOptions(jpeg_quality),
                              &messages_jpeg);

    impl::add_messages(messages_jpeg, messages);
    return success;
#else  // defined(SELENE_WITH_LIBJPEG)
    throw std::runtime_error("ERROR: JPEG writing unsupported; recompile with the respective external dependency.");
#endif  // defined(SELENE_WITH_LIBJPEG)
  }
  else if (format == ImageFormat::PNG)
  {
#if defined(SELENE_WITH_LIBPNG)
    MessageLog messages_png;
    bool success = write_png(dyn_img_view, std::forward<SinkType>(sink), PNGCompressionOptions(), &messages_png);

    impl::add_messages(messages_png, messages);
    return success;
#else  // defined(SELENE_WITH_LIBPNG)
    throw std::runtime_error("ERROR: PNG writing unsupported; recompile with the respective external dependency.");
#endif  // defined(SELENE_WITH_LIBPNG)
  }

  throw std::runtime_error("ERROR: Unknown image format.");
}

}  // namespace sln

#endif  // SELENE_IMG_IO_HPP

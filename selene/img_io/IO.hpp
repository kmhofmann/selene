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
#include <selene/img_io/TIFFRead.hpp>
#include <selene/img_io/TIFFWrite.hpp>

#include <stdexcept>
#include <variant>

namespace sln {

/** \brief Describes a supported image format for reading or writing.
 */
enum class ImageFormat
{
  JPEG,  ///< JPEG image format.
  PNG,   ///< PNG image format.
  TIFF,  ///< TIFF image format.
};

template <typename SourceType>
DynImage read_image(SourceType&& source, MessageLog* message_log = nullptr);

template <typename SinkType>
bool write_image(const DynImage& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log = nullptr,
                 const std::variant<std::monostate, JPEGCompressionOptions, PNGCompressionOptions, TIFFWriteOptions>& options = {});

template <ImageModifiability modifiability, typename SinkType>
bool write_image(const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log = nullptr,
                 const std::variant<std::monostate, JPEGCompressionOptions, PNGCompressionOptions, TIFFWriteOptions>& options = {});

// ----------
// Implementation:

namespace impl {

void add_messages(const MessageLog& message_log_src, MessageLog* message_log_dst);

template <typename SourceType>
[[nodiscard]] bool try_read_as_jpeg_image(SourceType&& source, DynImage& dyn_img, MessageLog* message_log)
{
  MessageLog message_log_jpeg;
  JPEGDecompressionObject obj;
  const auto header_info = read_jpeg_header(obj, std::forward<SourceType>(source), false, &message_log_jpeg);

  if (!header_info.is_valid())
  {
    return false;
  }

  dyn_img = read_jpeg(obj, std::forward<SourceType>(source), JPEGDecompressionOptions(), &message_log_jpeg,
                      &header_info);

  impl::add_messages(message_log_jpeg, message_log);
  return true;
}

template <typename SourceType>
[[nodiscard]] bool try_read_as_png_image(SourceType&& source, DynImage& dyn_img, MessageLog* message_log)
{
  MessageLog message_log_png;
  PNGDecompressionObject obj;
  const auto header_info = read_png_header(obj, std::forward<SourceType>(source), false, &message_log_png);

  if (!header_info.is_valid())
  {
    return false;
  }

  dyn_img = read_png(obj, std::forward<SourceType>(source), PNGDecompressionOptions(), &message_log_png,
                     &header_info);
  impl::add_messages(message_log_png, message_log);
  return true;
}

template <typename SourceType>
[[nodiscard]] bool try_read_as_tiff_image(SourceType&& source, DynImage& dyn_img, MessageLog* message_log)
{
  MessageLog message_log_tiff;
  TIFFReadObject<std::remove_reference_t<SourceType>> tiff_obj;
  const auto layouts = read_tiff_layouts(std::forward<SourceType>(source), &message_log_tiff, &tiff_obj);

  if (layouts.empty())  // We assume this means it's not a TIFF image...
  {
    return false;
  }

  dyn_img = read_tiff(std::forward<SourceType>(source), &message_log_tiff, &tiff_obj);
  impl::add_messages(message_log_tiff, message_log);
  return true;
}

}  // namespace impl

/** \brief Reads an image stream, trying all supported formats.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param message_log Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A `DynImage` instance. Reading the image stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename SourceType>
DynImage read_image(SourceType&& source,
                    [[maybe_unused]] MessageLog* message_log)
{
  bool reading_attempted = false;
  const auto source_pos = source.position();
  DynImage dyn_img;

  // First, try to read as JPEG image:
#if defined(SELENE_WITH_LIBJPEG)
  reading_attempted = true;
  const bool read_as_jpeg = impl::try_read_as_jpeg_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_jpeg)
  {
    return dyn_img;
  }
#endif  // defined(SELENE_WITH_LIBJPEG)

  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);

  // In case that failed, try to read as PNG image:
#if defined(SELENE_WITH_LIBPNG)
  reading_attempted = true;
  const bool read_as_png = impl::try_read_as_png_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_png)
  {
    return dyn_img;
  }
#endif  // defined(SELENE_WITH_LIBPNG)

  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);

  // In case that failed, try to read as TIFF image:
#if defined(SELENE_WITH_LIBTIFF)
  reading_attempted = true;
  const bool read_as_tiff = impl::try_read_as_tiff_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_tiff)
  {
    return dyn_img;
  }
#endif  // defined(SELENE_WITH_LIBTIFF)

  if (!reading_attempted)
  {
    throw std::runtime_error("ERROR: Image reading unsupported; recompile with the respective external dependencies.");
  }

  // Image reading failed: return an ImageData instance that is invalid:
  return dyn_img;
}

template <typename SinkType>
bool write_image(const DynImage& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log,
                 const std::variant<std::monostate, JPEGCompressionOptions, PNGCompressionOptions, TIFFWriteOptions>& options)
{
  return write_image(dyn_img.view(), format, sink, message_log, options);
}

/** \brief Writes an image stream, given the supplied uncompressed image data.
 *
 * @tparam modifiability The dynamic image modifiability (mutable or constant).
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param dyn_img The dynamic image to be written.
 * @param format Desired output image format.
 * @param sink Output sink instance.
 * @param message_log Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param jpeg_quality Compression quality for JPEG data. May take values from 1 (worst) to 100 (best). Only meaningful
 * in combination with `format` set to `ImageFormat::JPEG`, otherwise this value will be ignored.
 * @return True, if the write operation was successful; false otherwise.
 */
template <ImageModifiability modifiability, typename SinkType>
bool write_image([[maybe_unused]] const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 [[maybe_unused]] SinkType&& sink,
                 [[maybe_unused]] MessageLog* message_log,
                 [[maybe_unused]] const std::variant<std::monostate, JPEGCompressionOptions, PNGCompressionOptions, TIFFWriteOptions>& options)
{
  if (format == ImageFormat::JPEG)
  {
#if defined(SELENE_WITH_LIBJPEG)

    MessageLog message_log_jpeg;
    const auto options_jpeg = std::holds_alternative<JPEGCompressionOptions>(options)
                                ? std::get<JPEGCompressionOptions>(options) : JPEGCompressionOptions{};
    const bool success = write_jpeg(dyn_img_view, std::forward<SinkType>(sink), options_jpeg, &message_log_jpeg);

    impl::add_messages(message_log_jpeg, message_log);
    return success;

#else  // defined(SELENE_WITH_LIBJPEG)
    throw std::runtime_error("ERROR: JPEG writing unsupported; recompile with the respective external dependency.");
#endif  // defined(SELENE_WITH_LIBJPEG)
  }
  else if (format == ImageFormat::PNG)
  {
#if defined(SELENE_WITH_LIBPNG)

    MessageLog message_log_png;
    const auto options_png = std::holds_alternative<PNGCompressionOptions>(options)
                               ? std::get<PNGCompressionOptions>(options) : PNGCompressionOptions{};
    const bool success = write_png(dyn_img_view, std::forward<SinkType>(sink), options_png, &message_log_png);

    impl::add_messages(message_log_png, message_log);
    return success;

#else  // defined(SELENE_WITH_LIBPNG)
    throw std::runtime_error("ERROR: PNG writing unsupported; recompile with the respective external dependency.");
#endif  // defined(SELENE_WITH_LIBPNG)
  }
  else if (format == ImageFormat::TIFF)
  {
#if defined(SELENE_WITH_LIBTIFF)

    MessageLog message_log_tiff;
    const auto options_tiff = std::holds_alternative<TIFFWriteOptions>(options)
                                ? std::get<TIFFWriteOptions>(options) : TIFFWriteOptions{};
    const bool success = write_tiff(dyn_img_view, std::forward<SinkType>(sink), options_tiff, &message_log_tiff);

    impl::add_messages(message_log_tiff, message_log);
    return success;

#else  // defined(SELENE_WITH_LIBTIFF)
    throw std::runtime_error("ERROR: TIFF writing unsupported; recompile with the respective external dependency.");
#endif  // defined(SELENE_WITH_LIBTIFF)
  }

  throw std::runtime_error("ERROR: Unknown image format.");
}

}  // namespace sln

#endif  // SELENE_IMG_IO_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_IO_HPP
#define SELENE_IMG_IO_IO_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img_io/jpeg/Read.hpp>
#include <selene/img_io/jpeg/Write.hpp>
#include <selene/img_io/png/Read.hpp>
#include <selene/img_io/png/Write.hpp>
#include <selene/img_io/tiff/Read.hpp>
#include <selene/img_io/tiff/Write.hpp>

#include <stdexcept>
#include <variant>

namespace sln {

/// \addtogroup group-img-io
/// @{

/** \brief Describes a supported image format for reading or writing.
 */
enum class ImageFormat
{
#if defined(SELENE_WITH_LIBJPEG)
  JPEG,  ///< JPEG image format.
#endif
#if defined(SELENE_WITH_LIBPNG)
  PNG,   ///< PNG image format.
#endif
#if defined(SELENE_WITH_LIBTIFF)
  TIFF,  ///< TIFF image format.
#endif
};

template <typename Allocator = default_bytes_allocator, typename SourceType>
DynImage<Allocator> read_image(SourceType&& source, MessageLog* message_log = nullptr);

template <typename Allocator, typename SinkType>
bool write_image(const DynImage<Allocator>& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log = nullptr,
                 const std::variant<std::monostate
#if defined(SELENE_WITH_LIBJPEG)
                                    , JPEGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBPNG)
                                    , PNGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBTIFF)
                                    , TIFFWriteOptions
#endif
                                    >& options = {});

template <ImageModifiability modifiability, typename SinkType>
bool write_image(const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log = nullptr,
                 const std::variant<std::monostate
#if defined(SELENE_WITH_LIBJPEG)
                                    , JPEGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBPNG)
                                    , PNGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBTIFF)
                                    , TIFFWriteOptions
#endif
                                    >& options = {});

// ----------
// Implementation:

namespace impl {

void add_messages(const MessageLog& message_log_src, MessageLog* message_log_dst);

#if defined(SELENE_WITH_LIBJPEG)

template <typename Allocator, typename SourceType>
[[nodiscard]] bool try_read_as_jpeg_image(SourceType&& source, DynImage<Allocator>& dyn_img, MessageLog* message_log)
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

#endif  // defined(SELENE_WITH_LIBJPEG)

#if defined(SELENE_WITH_LIBPNG)

template <typename Allocator, typename SourceType>
[[nodiscard]] bool try_read_as_png_image(SourceType&& source, DynImage<Allocator>& dyn_img, MessageLog* message_log)
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

#endif  // defined(SELENE_WITH_LIBPNG)

#if defined(SELENE_WITH_LIBTIFF)

template <typename Allocator, typename SourceType>
[[nodiscard]] bool try_read_as_tiff_image(SourceType&& source, DynImage<Allocator>& dyn_img, MessageLog* message_log)
{
  const auto source_pos = source.position();

  MessageLog message_log_tiff;
  TIFFReadObject<std::remove_reference_t<SourceType>> tiff_obj;
  const auto layouts = read_tiff_layouts(std::forward<SourceType>(source), &message_log_tiff, &tiff_obj);

  source.seek_abs(source_pos);

  if (layouts.empty())  // We assume this means it's not a TIFF image...
  {
    return false;
  }

  dyn_img = read_tiff(std::forward<SourceType>(source), &message_log_tiff, &tiff_obj);
  impl::add_messages(message_log_tiff, message_log);
  return true;
}

#endif  // defined(SELENE_WITH_LIBTIFF)

}  // namespace impl

/** \brief Reads an image stream, trying all supported formats.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param message_log Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @return A `DynImage` instance. Reading the image stream was successful, if `is_valid() == true`, and unsuccessful
 * otherwise.
 */
template <typename Allocator, typename SourceType>
DynImage<Allocator> read_image(SourceType&& source,
                               [[maybe_unused]] MessageLog* message_log)
{
  [[maybe_unused]] bool reading_attempted = false;
  [[maybe_unused]] const auto source_pos = source.position();
  DynImage<Allocator> dyn_img;

  // First, try to read as JPEG image:

#if defined(SELENE_WITH_LIBJPEG)
  reading_attempted = true;
  const bool read_as_jpeg = impl::try_read_as_jpeg_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_jpeg)
  {
    return dyn_img;
  }

  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);
#endif  // defined(SELENE_WITH_LIBJPEG)

  // In case that failed, try to read as PNG image:

#if defined(SELENE_WITH_LIBPNG)
  reading_attempted = true;
  const bool read_as_png = impl::try_read_as_png_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_png)
  {
    return dyn_img;
  }

  SELENE_ASSERT(!dyn_img.is_valid());
  source.seek_abs(source_pos);
#endif  // defined(SELENE_WITH_LIBPNG)

  // In case that failed, try to read as TIFF image:

#if defined(SELENE_WITH_LIBTIFF)
  reading_attempted = true;
  const bool read_as_tiff = impl::try_read_as_tiff_image(std::forward<SourceType>(source), dyn_img, message_log);

  if (read_as_tiff)
  {
    return dyn_img;
  }
#endif  // defined(SELENE_WITH_LIBTIFF)

  if (!reading_attempted && message_log)
  {
    message_log->add("Image reading unsupported; recompile with the respective external dependencies.", MessageType::Error);
  }

  // Image reading failed: return an ImageData instance that is invalid:
  return dyn_img;
}

/** \brief Writes an image stream, given the supplied uncompressed image data (from a `DynImage`).
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param dyn_img The dynamic image to be written.
 * @param format Desired output image format.
 * @param sink Output sink instance.
 * @param message_log Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param options Writing/compression options for the respective format. If omitted, default options will be used.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename Allocator, typename SinkType>
bool write_image(const DynImage<Allocator>& dyn_img,
                 ImageFormat format,
                 SinkType&& sink,
                 MessageLog* message_log,
                 const std::variant<std::monostate
#if defined(SELENE_WITH_LIBJPEG)
                                    , JPEGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBPNG)
                                    , PNGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBTIFF)
                                    , TIFFWriteOptions
#endif
                                    >& options)
{
  return write_image(dyn_img.view(), format, sink, message_log, options);
}

/** \brief Writes an image stream, given the supplied uncompressed image data (from a `DynImageView`).
 *
 * @tparam modifiability The dynamic image modifiability (mutable or constant).
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param dyn_img_view The dynamic image view to be written.
 * @param format Desired output image format.
 * @param sink Output sink instance.
 * @param message_log Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param options Writing/compression options for the respective format. If omitted, default options will be used.
 * @return True, if the write operation was successful; false otherwise.
 */
template <ImageModifiability modifiability, typename SinkType>
bool write_image([[maybe_unused]] const DynImageView<modifiability>& dyn_img_view,
                 ImageFormat format,
                 [[maybe_unused]] SinkType&& sink,
                 [[maybe_unused]] MessageLog* message_log,
                 [[maybe_unused]] const std::variant<std::monostate
#if defined(SELENE_WITH_LIBJPEG)
                                                     , JPEGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBPNG)
                                                     , PNGCompressionOptions
#endif
#if defined(SELENE_WITH_LIBTIFF)
                                                     , TIFFWriteOptions
#endif
                                                     >& options)
{
#if defined(SELENE_WITH_LIBJPEG)
  if (format == ImageFormat::JPEG)
  {
    MessageLog message_log_jpeg;
    const auto options_jpeg = std::holds_alternative<JPEGCompressionOptions>(options)
                                ? std::get<JPEGCompressionOptions>(options) : JPEGCompressionOptions{};
    const bool success = write_jpeg(dyn_img_view, std::forward<SinkType>(sink), options_jpeg, &message_log_jpeg);

    impl::add_messages(message_log_jpeg, message_log);
    return success;
  }
#endif  // defined(SELENE_WITH_LIBJPEG)

#if defined(SELENE_WITH_LIBPNG)
  if (format == ImageFormat::PNG)
  {
    MessageLog message_log_png;
    const auto options_png = std::holds_alternative<PNGCompressionOptions>(options)
                               ? std::get<PNGCompressionOptions>(options) : PNGCompressionOptions{};
    const bool success = write_png(dyn_img_view, std::forward<SinkType>(sink), options_png, &message_log_png);

    impl::add_messages(message_log_png, message_log);
    return success;
  }
#endif  // defined(SELENE_WITH_LIBPNG)

#if defined(SELENE_WITH_LIBTIFF)
  if (format == ImageFormat::TIFF)
  {
    MessageLog message_log_tiff;
    const auto options_tiff = std::holds_alternative<TIFFWriteOptions>(options)
                                ? std::get<TIFFWriteOptions>(options) : TIFFWriteOptions{};
    const bool success = write_tiff(dyn_img_view, std::forward<SinkType>(sink), options_tiff, &message_log_tiff);

    impl::add_messages(message_log_tiff, message_log);
    return success;
  }
#endif  // defined(SELENE_WITH_LIBTIFF)

  if (message_log)
  {
    message_log->add("Unknown or unsupported image format.", MessageType::Error);
  }

  return false;
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_IO_IO_HPP

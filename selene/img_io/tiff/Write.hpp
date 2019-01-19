// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_TIFF_WRITE_HPP
#define SELENE_IMG_IO_TIFF_WRITE_HPP

/// @file

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/_impl/StaticChecks.hpp>

#include <selene/img_io/tiff/Common.hpp>

#include <memory>

namespace sln {

/// \addtogroup group-img-io-tiff
/// @{

template <typename SinkType> class TIFFWriteObject;
template <typename SinkType> class TIFFWriter;

/** \brief Options for TIFF writing, controlling (parts of) the output format.
 *
 * In particular, the compression type can be specified, e.g. using LZW or JPEG compression. The respective compression
 * type must be supported by the installed *libtiff* library.
 */
struct TIFFWriteOptions
{
  enum class Layout
  {
    Strips,
    Tiles,
  };

  TIFFCompression compression_type;  ///< The compression type for the TIFF data.
  int jpeg_quality;  ///< The JPEG quality (0...100), in case `compression_type == TIFFCompression::JPEG`.
  TIFFWriteOptions::Layout layout;  ///< The storage layout to use.

  std::size_t max_bytes_per_strip{64 * 1024};
  std::size_t nr_rows_per_strip{4096};
  std::size_t tile_width{256};
  std::size_t tile_height{256};

  explicit TIFFWriteOptions(TIFFCompression compression_type_ = TIFFCompression::None,
                            int jpeg_quality_ = 95,
                            TIFFWriteOptions::Layout layout_ = TIFFWriteOptions::Layout::Strips)
      : compression_type(compression_type_)
      , jpeg_quality(jpeg_quality_)
      , layout(layout_)
  {
  }
};

template <typename DynImageOrView, typename SinkType>
bool write_tiff(const DynImageOrView& dyn_img_or_view,
                SinkType&& sink,
                const TIFFWriteOptions& write_options = TIFFWriteOptions(),
                MessageLog* message_log = nullptr,
                TIFFWriteObject<std::remove_reference_t<SinkType>>* = nullptr);


namespace impl {
template <typename SinkType, typename DynImageOrView>
    bool tiff_write_to_current_directory(TIFFWriteObject<SinkType>&, const TIFFWriteOptions&, MessageLog&,
                                         const DynImageOrView&, std::ptrdiff_t = -1);
}  // namespace impl

/** \brief Opaque TIFF writing object, holding internal state.
 */
template <typename SinkType>
class TIFFWriteObject
{
public:
  /// \cond INTERNAL
  TIFFWriteObject();
  ~TIFFWriteObject();
  TIFFWriteObject(const TIFFWriteObject&) = delete;
  TIFFWriteObject& operator=(const TIFFWriteObject&) = delete;
  TIFFWriteObject(TIFFWriteObject&&) noexcept = default;
  TIFFWriteObject& operator=(TIFFWriteObject&&) noexcept = default;
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  bool open(SinkType& sink);
  bool open(SinkType&& sink);
  bool write_directory();
  bool flush();
  void close();

  template <typename DynImageOrView, typename SinkType2> friend bool write_tiff(const DynImageOrView&, SinkType2&&, const TIFFWriteOptions&, MessageLog*, TIFFWriteObject<std::remove_reference_t<SinkType2>>*);
  template <typename SinkType2, typename DynImageOrView> friend bool impl::tiff_write_to_current_directory(TIFFWriteObject<SinkType2>&, const TIFFWriteOptions&, MessageLog&, const DynImageOrView&, std::ptrdiff_t);

  friend class TIFFWriter<SinkType>;
};

/** \brief Class with functionality to write a TIFF image data stream.
 *
 * Generally, the free function write_tiff() should be preferred, due to ease of use.
 *
 * This function, however, does not allow writing multiple images into a single TIFF file (sometimes also called
 * multiple 'directories', or a 'multi-page' TIFF).
 *
 * This can be achieved by attaching a data sink to a `TIFFWriter` instance, and then calling `write_image_data`
 * repeatedly, in the order that images should be written.
 * At the end, to properly flush/close the TIFF stream, `finish_writing` needs to be called; this also happens when
 * the `TIFFWriter` instance goes out of scope.
 *
 * Any errors will be written to an internal `MessageLog` instance, which can be queried via the `message_log`
 * function.
 *
 *
 *
 *
 * This is enabled by calling `read_layouts()` on an instance of this class, then allocating the respective
 * `DynImage` instance(s) (or by providing a `DynImageView` into pre-allocated memory), and finally calling
 * `read_image_data(DynImage&)` or `read_image_data(MutableDynImageView&)` on each TIFF directory.
 * TIFF directories can be advanced one by one using the `advance_directory()` member function, or alternatively set
 * to one of the contained directories by calling `set_directory` with the respective index.
 *
 * Multiple images can be read using the same TIFFReader<> (on the same thread).
 * The source will need to be re-set using `set_source()` to the respective position before attempting to read a new
 * image; there is no guarantee that after reading image data, the stream pointer will point past the end of the TIFF
 * file.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 */
template <typename SinkType>
class TIFFWriter
{
public:
  TIFFWriter() = default;
  explicit TIFFWriter(SinkType& sink);

  void set_sink(SinkType& sink);

  template <typename DynImageOrView>
      bool write_image_data(const DynImageOrView& dyn_img_or_view,
                            const TIFFWriteOptions& options = TIFFWriteOptions{});
  void finish_writing();

  MessageLog& message_log();

private:
  SinkType* sink_{nullptr};
  TIFFWriteObject<SinkType> write_object_;
  MessageLog message_log_;
  std::ptrdiff_t nr_images_written{0};
};

/// @}

// ----------
// Implementation:

/** \brief Write a TIFF image data stream, given the supplied uncompressed image data.
 *
 * @tparam DynImageOrView The type of the input image data. Can be of type `DynImage` or `DynImageView<>`.
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param dyn_img_or_view The dynamic image (view) to be written.
 * @param sink Output sink instance.
 * @param write_options Options for writing the TIFF image.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFWriteObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations.
 * @return True, if the write operation was successful; false otherwise.
 */
template <typename DynImageOrView, typename SinkType>
bool write_tiff(const DynImageOrView& dyn_img_or_view,
                SinkType&& sink,
                const TIFFWriteOptions& write_options,
                MessageLog* message_log,
                TIFFWriteObject<std::remove_reference_t<SinkType>>* tiff_object)
{
  impl::static_check_is_dyn_image_or_view<DynImageOrView>();

  impl::tiff_set_handlers();
  TIFFWriteObject<std::remove_reference_t<SinkType>> local_tiff_object;
  TIFFWriteObject<std::remove_reference_t<SinkType>>* obj = tiff_object ? tiff_object : &local_tiff_object;

  MessageLog local_message_log;

  if (!obj->open(std::forward<SinkType>(sink)))
  {
    local_message_log.add("TIFF writer: ERROR: Data stream could not be opened.", MessageType::Error);
    impl::tiff_assign_message_log(local_message_log, message_log);
    return false;
  }

  const bool success = impl::tiff_write_to_current_directory(*obj, write_options, local_message_log, dyn_img_or_view);
  const bool flushed = obj->flush();

  impl::tiff_assign_message_log(local_message_log, message_log);
  return success && flushed;
}

// -----

/** \brief Constructs a TIFFReader instance with the given data stream source.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param sink Output sink instance.
 */
template <typename SinkType>
TIFFWriter<SinkType>::TIFFWriter(SinkType& sink)
    : sink_(&sink)
{
  impl::tiff_set_handlers();
  write_object_.open(*sink_);
}

/** \brief Sets an output sink stream.
 *
 * @tparam SinkType Type of the output sink. Can be FileWriter or VectorWriter.
 * @param sink Output sink instance.
 */
template <typename SinkType>
void TIFFWriter<SinkType>::set_sink(SinkType& sink)
{
  impl::tiff_set_handlers();
  sink_ = &sink;
  write_object_.open(*sink_);
  nr_images_written = 0;
}

template <typename SinkType>
template <typename DynImageOrView>
bool TIFFWriter<SinkType>::write_image_data(const DynImageOrView& dyn_img_or_view, const TIFFWriteOptions& options)
{
  if (sink_ == nullptr)
  {
    message_log_.add("TIFFWriter sink is not set.", MessageType::Error);
    return false;
  }

  const bool success = impl::tiff_write_to_current_directory(write_object_, options, message_log_, dyn_img_or_view,
                                                             nr_images_written);

  [[maybe_unused]] const bool write_dir = write_object_.write_directory();
  SELENE_ASSERT(write_dir);

  ++nr_images_written;
  return success;
}

template <typename SinkType>
void TIFFWriter<SinkType>::finish_writing()
{
  write_object_.close();
}

template <typename SinkType>
MessageLog& TIFFWriter<SinkType>::message_log()
{
  return message_log_;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_WRITE_HPP
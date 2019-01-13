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

template <typename SinkType> class TIFFWriteObject;

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
    bool tiff_write_to_current_directory(TIFFWriteObject<SinkType>&,
                                         const TIFFWriteOptions&,
                                         MessageLog&,
                                         const DynImageOrView&);
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
  bool flush();

  template <typename DynImageOrView, typename SinkType2> friend bool write_tiff(const DynImageOrView&, SinkType2&&, const TIFFWriteOptions&, MessageLog*, TIFFWriteObject<std::remove_reference_t<SinkType2>>*);
  template <typename SinkType2, typename DynImageOrView> friend bool impl::tiff_write_to_current_directory(TIFFWriteObject<SinkType2>&, const TIFFWriteOptions&, MessageLog&, const DynImageOrView&);
};

// TODO: Implement TIFFWriter interface.

// ----------
// Implementation:

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

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_WRITE_HPP
// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_TIFF_READ_HPP
#define SELENE_IMG_IO_TIFF_READ_HPP

/// @file

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img_io/tiff/Common.hpp>

#include <memory>
#include <vector>

namespace sln {

/// \addtogroup group-img-io-tiff
/// @{

template <typename SourceType> class TIFFReadObject;
template <typename SourceType> class TIFFReader;

template <typename SourceType>
std::vector<TiffImageLayout> read_tiff_layouts(SourceType&&, MessageLog* = nullptr, TIFFReadObject<std::remove_reference_t<SourceType>>* = nullptr);

template <typename SourceType>
DynImage read_tiff(SourceType&&, MessageLog* = nullptr, TIFFReadObject<std::remove_reference_t<SourceType>>* = nullptr);

template <typename SourceType>
std::vector<DynImage> read_tiff_all(SourceType&&, MessageLog* = nullptr, TIFFReadObject<std::remove_reference_t<SourceType>>* = nullptr);


namespace impl {
template <typename SourceType, typename DynImageOrView>
    [[nodiscard]] bool tiff_read_current_directory(TIFFReadObject<SourceType>& tiff_obj,
                                                   MessageLog& message_log,
                                                   DynImageOrView& dyn_img_or_view);
}  // namespace impl

/** \brief Opaque TIFF reading object, holding internal state.
 */
template <typename SourceType>
class TIFFReadObject
{
public:
  /// \cond INTERNAL
  TIFFReadObject();
  ~TIFFReadObject();
  TIFFReadObject(const TIFFReadObject&) = delete;
  TIFFReadObject& operator=(const TIFFReadObject&) = delete;
  TIFFReadObject(TIFFReadObject&&) noexcept = default;
  TIFFReadObject& operator=(TIFFReadObject&&) noexcept = default;
  /// \endcond

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  bool open(SourceType& source);
  bool open(SourceType&& source);
  TiffImageLayout get_layout();
  bool advance_directory();
  bool set_directory(std::uint16_t index);

  template <typename SourceType2> friend std::vector<TiffImageLayout> read_tiff_layouts(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);
  template <typename SourceType2> friend DynImage read_tiff(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);
  template <typename SourceType2> friend std::vector<DynImage> read_tiff_all(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);

  template <typename SourceType2, typename DynImageOrView> friend bool impl::tiff_read_current_directory(TIFFReadObject<SourceType2>&, MessageLog&, DynImageOrView&);

  friend class TIFFReader<SourceType>;
};

/** \brief Class with functionality to read header and data of a TIFF image data stream.
 *
 * Generally, the free functions read_tiff() or read_tiff_all() should be preferred, due to ease of use.
 *
 * Both of these, however, do not allow reading of the decompressed image data into pre-allocated memory.
 * This is enabled by calling read_layouts() on an instance of this class, then allocating the respective
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
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 */
template <typename SourceType>
class TIFFReader
{
public:
  TIFFReader() = default;
  explicit TIFFReader(SourceType& source);

  void set_source(SourceType& source);

  std::vector<TiffImageLayout> read_layouts();
  bool advance_directory();
  bool set_directory(std::size_t index);

  DynImage read_image_data();
  template <typename DynImageOrView> bool read_image_data(DynImageOrView& dyn_img_or_view);

  MessageLog& message_log();

private:
  SourceType* source_{nullptr};
  TIFFReadObject<SourceType> read_object_;
  MessageLog message_log_;
};

/// @}

// ----------
// Implementation:

/** \brief Read and return the layouts of all contained images within a TIFF data stream/file.
 *
 * TIFF files may contain more than one image.
 * This function reads and returns the layout information of all images within the TIFF file, without reading any
 * image data.
 *
 * The size of the output vector corresponds to the number of images contained in the TIFF data stream.
 * In case the returned vector is empty, the supplied data stream was likely not in the TIFF format.
 *
 * After reading, the source position will point to the beginning of the TIFF data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations.
 * @return A vector with all read TIFF image layouts.
 */
template <typename SourceType>
std::vector<TiffImageLayout> read_tiff_layouts(SourceType&& source, MessageLog* message_log, TIFFReadObject<std::remove_reference_t<SourceType>>* tiff_object)
{
  impl::tiff_set_handlers();
  TIFFReadObject<std::remove_reference_t<SourceType>> local_tiff_object;
  TIFFReadObject<std::remove_reference_t<SourceType>>* obj = tiff_object ? tiff_object : &local_tiff_object;

  MessageLog local_message_log;
  std::vector<TiffImageLayout> layouts;

  SELENE_ASSERT(source.is_open());
  const auto start_pos = source.position();

  if (!obj->open(std::forward<SourceType>(source)))
  {
    local_message_log.add("Data stream could not be opened.", MessageType::Error);
    impl::tiff_assign_message_log(local_message_log, message_log);
    return layouts;
  }

  do
  {
    layouts.push_back(obj->get_layout());
  } while (obj->advance_directory());

  source.seek_abs(start_pos);

  return layouts;
}

/** \brief Read the first TIFF image within a file.
 *
 * TIFF files may contain more than one image.
 * This function reads the *first* TIFF image from the supplied data stream (here denoted as a 'file'), and returns it.
 *
 * After reading, the source position may *not* point past the end of the TIFF data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations.
 * @return The read TIFF image from the data stream/file. In case the image could not be read successfully, it will not
 * be valid (i.e. `is_valid() == false`).
 */
template <typename SourceType>
DynImage read_tiff(SourceType&& source, MessageLog* message_log, TIFFReadObject<std::remove_reference_t<SourceType>>* tiff_object)
{
  impl::tiff_set_handlers();
  TIFFReadObject<std::remove_reference_t<SourceType>> local_tiff_object;
  TIFFReadObject<std::remove_reference_t<SourceType>>* obj = tiff_object ? tiff_object : &local_tiff_object;

  MessageLog local_message_log;

  SELENE_ASSERT(source.is_open());

  if (!obj->open(std::forward<SourceType>(source)))
  {
    local_message_log.add("Data stream could not be opened.", MessageType::Error);
    impl::tiff_assign_message_log(local_message_log, message_log);
    return DynImage{};
  }

  DynImage dyn_img;
  [[maybe_unused]] const bool read_successfully = impl::tiff_read_current_directory(*obj, local_message_log, dyn_img);

  impl::tiff_assign_message_log(local_message_log, message_log);
  return dyn_img;
}

/** \brief Read all TIFF images within a file.
 *
 * TIFF files may contain more than one image.
 * This function reads all TIFF images from the supplied data stream (here denoted as a 'file'), and returns them.
 *
 * After reading, the source position may *not* point past the end of the TIFF data stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations.
 * @return A vector with all read TIFF images from the data stream/file. Images that could not be read successfully
 * will not be valid (i.e. `is_valid() == false`).
 */
template <typename SourceType>
std::vector<DynImage> read_tiff_all(SourceType&& source, MessageLog* message_log, TIFFReadObject<std::remove_reference_t<SourceType>>* tiff_object)
{
  impl::tiff_set_handlers();
  TIFFReadObject<std::remove_reference_t<SourceType>> local_tiff_object;
  TIFFReadObject<std::remove_reference_t<SourceType>>* obj = tiff_object ? tiff_object : &local_tiff_object;

  MessageLog local_message_log;
  std::vector<DynImage> images;

  SELENE_ASSERT(source.is_open());

  if (!obj->open(source))
  {
    local_message_log.add("Data stream could not be opened.", MessageType::Error);
    impl::tiff_assign_message_log(local_message_log, message_log);
    return images;
  }

  do
  {
    DynImage dyn_img;
    [[maybe_unused]] const bool read_successfully = impl::tiff_read_current_directory(*obj, local_message_log, dyn_img);
    images.push_back(std::move(dyn_img));
  } while (obj->advance_directory());

  impl::tiff_assign_message_log(local_message_log, message_log);
  return images;
}

// -----

/** \brief Constructs a TIFFReader instance with the given data stream source.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 */
template <typename SourceType>
TIFFReader<SourceType>::TIFFReader(SourceType& source)
    : source_(&source)
{
  impl::tiff_set_handlers();
  read_object_.open(*source_);
}

/** \brief Sets an input source stream.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 */
template <typename SourceType>
void TIFFReader<SourceType>::set_source(SourceType& source)
{
  impl::tiff_set_handlers();
  source_ = &source;
  read_object_.open(*source_);
}

template <typename SourceType>
std::vector<TiffImageLayout> TIFFReader<SourceType>::read_layouts()
{
  if (source_ == nullptr)
  {
    message_log_.add("TIFFReader source is not set.", MessageType::Error);
    return {};
  }

  std::vector<TiffImageLayout> layouts;
  const auto start_pos = source_->position();

  do
  {
    layouts.push_back(read_object_.get_layout());
  } while (read_object_.advance_directory());

  source_->seek_abs(start_pos);
  return layouts;
}

template <typename SourceType>
bool TIFFReader<SourceType>::advance_directory()
{
  return read_object_.advance_directory();
}

template <typename SourceType>
bool TIFFReader<SourceType>::set_directory(std::size_t index)
{
  return read_object_.set_directory(static_cast<std::uint16_t>(index));
}

template <typename SourceType>
DynImage TIFFReader<SourceType>::read_image_data()
{
  if (source_ == nullptr)
  {
    message_log_.add("TIFFReader source is not set.", MessageType::Error);
    return DynImage{};
  }

  DynImage dyn_img;
  [[maybe_unused]] const bool success = impl::tiff_read_current_directory(read_object_, message_log_, dyn_img);
  return dyn_img;
}

template <typename SourceType>
template <typename DynImageOrView>
bool TIFFReader<SourceType>::read_image_data(DynImageOrView& dyn_img_or_view)
{
  if (source_ == nullptr)
  {
    message_log_.add("TIFFReader source is not set.", MessageType::Error);
    return false;
  }

  const bool success = impl::tiff_read_current_directory(read_object_, message_log_, dyn_img_or_view);
  return success;
}

template <typename SourceType>
MessageLog& TIFFReader<SourceType>::message_log()
{
  return message_log_;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_READ_HPP
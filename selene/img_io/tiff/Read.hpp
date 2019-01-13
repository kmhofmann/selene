// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_TIFF_READ_HPP
#define SELENE_IMG_IO_TIFF_READ_HPP

/// @file

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img_io/tiff/Common.hpp>

#include <memory>
#include <vector>

namespace sln {

template <typename SourceType> class TIFFReadObject;

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
  int set_directory(std::uint16_t index);

  template <typename SourceType2> friend std::vector<TiffImageLayout> read_tiff_layouts(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);
  template <typename SourceType2> friend DynImage read_tiff(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);
  template <typename SourceType2> friend std::vector<DynImage> read_tiff_all(SourceType2&&, MessageLog*, TIFFReadObject<std::remove_reference_t<SourceType2>>*);

  template <typename SourceType2, typename DynImageOrView> friend bool impl::tiff_read_current_directory(TIFFReadObject<SourceType2>&, MessageLog&, DynImageOrView&);
};

// TODO: Implement TIFFReader interface.

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
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations. Note that the same TIFF reader object can **only** be
 * re-used with the same `source` instance throughout its lifetime! Any other use is undefined.
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

  // Reset back to first image
  obj->set_directory(0);

  return layouts;
}

/** \brief Read the first TIFF image within a file.
 *
 * TIFF files may contain more than one image.
 * This function reads the *first* TIFF image from the supplied data stream (here denoted as a 'file'), and returns it.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations. Note that the same TIFF reader object can **only** be
 * re-used with the same `source` instance throughout its lifetime! Any other use is undefined.
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

  // Reset back to first image
  obj->set_directory(0);

  impl::tiff_assign_message_log(local_message_log, message_log);
  return dyn_img;
}

/** \brief Read all TIFF images within a file.
 *
 * TIFF files may contain more than one image.
 * This function reads all TIFF images from the supplied data stream (here denoted as a 'file'), and returns them.
 *
 * @tparam SourceType Type of the input source. Can be FileReader or MemoryReader.
 * @param source Input source instance.
 * @param messages Optional pointer to the message log. If provided, warning and error messages will be output there.
 * @param tiff_object Optional TIFFReadObject instance, which can be explicitly instantiated outside of this function.
 * Providing this may save internal memory (de)allocations. Note that the same TIFF reader object can **only** be
 * re-used with the same `source` instance throughout its lifetime! Any other use is undefined.
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

  // Reset back to first image
  obj->set_directory(0);

  impl::tiff_assign_message_log(local_message_log, message_log);
  return images;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_TIFF_READ_HPP
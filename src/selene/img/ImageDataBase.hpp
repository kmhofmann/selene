// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_BASE_HPP
#define SELENE_IMG_IMAGE_DATA_BASE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/ImageDataStorage.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>
#include <selene/img/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace sln {

/** \brief Base class for `ImageData`. Should not be instantiated by a user, but is used by the
 * implementation of `ImageData`.
 *
 * @tparam DataStoragePtr The type of pointer for data storage.
 */
template <typename DataStoragePtr>
class ImageDataBase
{
public:
  PixelLength width() const noexcept;
  PixelLength height() const noexcept;
  std::uint16_t nr_channels() const noexcept;
  std::uint16_t nr_bytes_per_channel() const noexcept;
  Stride stride_bytes() const noexcept;
  std::size_t row_bytes() const noexcept;
  std::size_t total_bytes() const noexcept;
  PixelFormat pixel_format() const noexcept;
  SampleFormat sample_format() const noexcept;
  bool is_packed() const noexcept;
  bool is_view() const noexcept;
  bool is_empty() const noexcept;
  bool is_valid() const noexcept;

  void clear() noexcept;

  void set_view(DataStoragePtr data,
                PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown);

  void set_pixel_format(PixelFormat pixel_format);
  void set_sample_format(SampleFormat sample_format);

  const std::uint8_t* byte_ptr() const noexcept;
  const std::uint8_t* byte_ptr(PixelIndex y) const noexcept;
  const std::uint8_t* byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType>
  const PixelType* data() const noexcept;

  template <typename PixelType>
  const PixelType* data(PixelIndex y) const noexcept;

  template <typename PixelType>
  const PixelType* data_row_end(PixelIndex y) const noexcept;

  template <typename PixelType>
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType>
  const PixelType& pixel(PixelIndex x, PixelIndex y) const noexcept;

protected:
  /// \cond INTERNAL
  DataStoragePtr data_ = nullptr;
  PixelLength width_ = 0_px;
  PixelLength height_ = 0_px;
  Stride stride_bytes_ = Stride{0};
  std::uint16_t nr_channels_ = 0;
  std::uint16_t nr_bytes_per_channel_ = 0;
  PixelFormat pixel_format_ = PixelFormat::Unknown;
  SampleFormat sample_format_ = SampleFormat::Unknown;

  ImageDataBase() = default;

  void reset();
  Bytes compute_data_offset(PixelIndex y) const noexcept;
  Bytes compute_data_offset(PixelIndex x, PixelIndex y) const noexcept;
  /// \endcond
};

/** \brief Returns the image width.
 *
 * @return Width of the image in pixels.
 */
template <typename DataStoragePtr>
inline PixelLength ImageDataBase<DataStoragePtr>::width() const noexcept
{
  return width_;
}

/** \brief Returns the image height.
 *
 * @return Height of the image in pixels.
 */
template <typename DataStoragePtr>
inline PixelLength ImageDataBase<DataStoragePtr>::height() const noexcept
{
  return height_;
}

/** \brief Returns the number of image channels.
 *
 * @return Number of image channels.
 */
template <typename DataStoragePtr>
inline std::uint16_t ImageDataBase<DataStoragePtr>::nr_channels() const noexcept
{
  return nr_channels_;
}

/** \brief Returns the number of bytes stored for each sample, per image channel.
 *
 * @return Number of bytes stored for each sample, per image channel.
 */
template <typename DataStoragePtr>
inline std::uint16_t ImageDataBase<DataStoragePtr>::nr_bytes_per_channel() const noexcept
{
  return nr_bytes_per_channel_;
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the number of bytes for each pixel element:
 * `(stride_bytes() >= width() * nr_channels() * nr_bytes_per_channel())`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @return Row stride in bytes.
 */
template <typename DataStoragePtr>
inline Stride ImageDataBase<DataStoragePtr>::stride_bytes() const noexcept
{
  return stride_bytes_;
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * nr_channels() * nr_bytes_per_channel())`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @return Number of data bytes occupied by each image row.
 */
template <typename DataStoragePtr>
inline std::size_t ImageDataBase<DataStoragePtr>::row_bytes() const noexcept
{
  return width_ * nr_channels_ * nr_bytes_per_channel_;
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @return Number of bytes occupied by the image data in memory.
 */
template <typename DataStoragePtr>
inline std::size_t ImageDataBase<DataStoragePtr>::total_bytes() const noexcept
{
  return stride_bytes_ * height_;
}

/** \brief Returns the pixel format (semantic tag).
 *
 * @return The pixel format.
 */
template <typename DataStoragePtr>
inline PixelFormat ImageDataBase<DataStoragePtr>::pixel_format() const noexcept
{
  return pixel_format_;
}

/** \brief Returns the sample format (semantic tag).
 *
 * @return The sample format.
 */
template <typename DataStoragePtr>
inline SampleFormat ImageDataBase<DataStoragePtr>::sample_format() const noexcept
{
  return sample_format_;
}

/** \brief Returns whether the image data is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * nr_channels() * nr_bytes_per_channel())`.
 *
 * @return True, if the image data stored packed; false otherwise.
 */
template <typename DataStoragePtr>
inline bool ImageDataBase<DataStoragePtr>::is_packed() const noexcept
{
  return stride_bytes_ == nr_bytes_per_channel_ * nr_channels_ * width_;
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
template <typename DataStoragePtr>
inline bool ImageDataBase<DataStoragePtr>::is_view() const noexcept
{
  return true;
}

/** \brief Returns whether the image is empty.
 *
 * An image is considered empty if its internal data pointer points to `nullptr`, `width() == 0`, `height() == 0`,
 * `nr_channels() == 0`, `nr_bytes_per_channel() == 0`, or any combination of these.
 *
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename DataStoragePtr>
inline bool ImageDataBase<DataStoragePtr>::is_empty() const noexcept
{
  return data_ == nullptr || width_ == 0 || height_ == 0 || nr_channels_ == 0 || nr_bytes_per_channel_ == 0;
}

/** \brief Returns whether the instance represents a valid image.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @return True, if the image is valid; false otherwise.
 */
template <typename DataStoragePtr>
inline bool ImageDataBase<DataStoragePtr>::is_valid() const noexcept
{
  return !is_empty();
}

/** \brief Resets the image instance by clearing the image data and resetting the internal state to the state after
 * default construction.
 *
 * Postconditions: `data() == nullptr && width() == 0 && height() == 0 && stride_bytes() == 0 && nr_channels() == 0 &&
 * nr_bytes_per_channel() == 0 && pixel_format() == PixelFormat::Unknown && output_sample_format() ==
 * SampleType::Unknown && is_empty() && !is_valid() && !is_view()`.
 */
template <typename DataStoragePtr>
inline void ImageDataBase<DataStoragePtr>::clear() noexcept
{
  reset();
}

/** \brief Sets the image data to be a view onto non-owned external memory.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * Postcondition: `is_view()`.
 *
 * @param data Pointer to the external data.
 * @param width The image width.
 * @param height The image height.
 * @param nr_channels The number of channels for each pixel element.
 * @param nr_bytes_per_channel The number of bytes per channel in each pixel element.
 * @param stride_bytes The row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
template <typename DataStoragePtr>
inline void ImageDataBase<DataStoragePtr>::set_view(DataStoragePtr data,
                                                    PixelLength width,
                                                    PixelLength height,
                                                    std::uint16_t nr_channels,
                                                    std::uint16_t nr_bytes_per_channel,
                                                    Stride stride_bytes,
                                                    PixelFormat pixel_format,
                                                    SampleFormat sample_format)
{
  stride_bytes = std::max(stride_bytes, Stride(nr_bytes_per_channel * nr_channels * width));

  data_ = data;
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_format_ = sample_format;
}

/** \brief Sets the pixel format.
 *
 * @param pixel_format The pixel format to be set.
 */
template <typename DataStoragePtr>
inline void ImageDataBase<DataStoragePtr>::set_pixel_format(PixelFormat pixel_format)
{
  pixel_format_ = pixel_format;
}

/** \brief Sets the sample format.
 *
 * @param pixel_format The sample format to be set.
 */
template <typename DataStoragePtr>
inline void ImageDataBase<DataStoragePtr>::set_sample_format(SampleFormat sample_format)
{
  sample_format_ = sample_format;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @return Constant pointer to the first image data byte.
 */
template <typename DataStoragePtr>
inline const std::uint8_t* ImageDataBase<DataStoragePtr>::byte_ptr() const noexcept
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename DataStoragePtr>
inline const std::uint8_t* ImageDataBase<DataStoragePtr>::byte_ptr(PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column
 * `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename DataStoragePtr>
inline const std::uint8_t* ImageDataBase<DataStoragePtr>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * Due to the "dynamically" typed image data, the pixel type has to be determined at function call granularity.
 * Compatibility with the data is checked only via debug-mode assertions.
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename DataStoragePtr>
template <typename PixelType>
const PixelType* ImageDataBase<DataStoragePtr>::data() const noexcept
{
  SELENE_ASSERT(nr_channels_ == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(nr_bytes_per_channel_ == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(sample_format_ == SampleFormat::Unknown || sample_format_ == PixelTraits<PixelType>::sample_format);

  return reinterpret_cast<const PixelType*>(byte_ptr());
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * Due to the "dynamically" typed image data, the pixel type has to be determined at function call granularity.
 * Compatibility with the data is checked only via debug-mode assertions.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename DataStoragePtr>
template <typename PixelType>
const PixelType* ImageDataBase<DataStoragePtr>::data(PixelIndex y) const noexcept
{
  SELENE_ASSERT(nr_channels_ == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(nr_bytes_per_channel_ == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(sample_format_ == SampleFormat::Unknown || sample_format_ == PixelTraits<PixelType>::sample_format);

  return reinterpret_cast<const PixelType*>(byte_ptr(y));
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y,
 * column `width()`).
 *
 * Due to the "dynamically" typed image data, the pixel type has to be determined at function call granularity.
 * Compatibility with the data is checked only via debug-mode assertions.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename DataStoragePtr>
template <typename PixelType>
const PixelType* ImageDataBase<DataStoragePtr>::data_row_end(PixelIndex y) const noexcept
{
  SELENE_ASSERT(nr_channels_ == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(nr_bytes_per_channel_ == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(sample_format_ == SampleFormat::Unknown || sample_format_ == PixelTraits<PixelType>::sample_format);

  return reinterpret_cast<const PixelType*>(byte_ptr(y) + nr_bytes_per_channel_ * nr_channels_ * width_);
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * Due to the "dynamically" typed image data, the pixel type has to be determined at function call granularity.
 * Compatibility with the data is checked only via debug-mode assertions.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename DataStoragePtr>
template <typename PixelType>
const PixelType* ImageDataBase<DataStoragePtr>::data(PixelIndex x, PixelIndex y) const noexcept
{
  SELENE_ASSERT(nr_channels_ == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(nr_bytes_per_channel_ == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(sample_format_ == SampleFormat::Unknown || sample_format_ == PixelTraits<PixelType>::sample_format);

  return reinterpret_cast<const PixelType*>(byte_ptr(x, y));
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * Due to the "dynamically" typed image data, the pixel type has to be determined at function call granularity.
 * Compatibility with the data is checked only via debug-mode assertions.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename DataStoragePtr>
template <typename PixelType>
inline const PixelType& ImageDataBase<DataStoragePtr>::pixel(PixelIndex x, PixelIndex y) const noexcept
{
  return *data<PixelType>(x, y);
}

template <typename DataStoragePtr>
inline void ImageDataBase<DataStoragePtr>::reset()
{
  // reset to default constructed state
  data_ = nullptr;
  width_ = PixelLength{0};
  height_ = PixelLength{0};
  stride_bytes_ = Stride{0};
  nr_channels_ = std::uint16_t{0};
  nr_bytes_per_channel_ = std::uint16_t{0};
  pixel_format_ = PixelFormat::Unknown;
  sample_format_ = SampleFormat::Unknown;
}

template <typename DataStoragePtr>
inline Bytes ImageDataBase<DataStoragePtr>::compute_data_offset(PixelIndex y) const noexcept
{
  return Bytes(stride_bytes_ * y);
}

template <typename DataStoragePtr>
inline Bytes ImageDataBase<DataStoragePtr>::compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
{
  return Bytes(stride_bytes_ * y + nr_bytes_per_channel_ * nr_channels_ * x);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_DATA_BASE_HPP

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_HPP
#define SELENE_IMG_IMAGE_DATA_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/ImageDataBase.hpp>
#include <selene/img/ImageDataStorage.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace sln {

template <typename T>
class Image;

/** \brief Dynamically typed image class.
 *
 * An instance of `ImageData` represents a dynamically typed image with pixel elements in interleaved storage.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * Each image pixel can have an arbitrary number of channels, and each channel/sample in a pixel can have an arbitrary
 * number of bytes.
 *
 * Optionally, an image can have be tagged with a particular `PixelFormat` or a particular `SampleType`.
 * This is mostly a semantic tag and has little influence on the data content.
 *
 * The memory of an `ImageData` instance may either be owned or non-owned; in the latter case, the instance is a "view"
 * on image data.
 */
template <ImageDataStorage storage_type = ImageDataStorage::Modifiable>
class ImageData;

/** \brief Explicit specialization of `ImageData` for constant image data.
 *
 * Can only point to non-owned memory, i.e. describe views onto constant image data.
 */
template <>
class ImageData<ImageDataStorage::Constant> : public ImageDataBase<const std::uint8_t*>
{
public:
  ImageData() = default;  ///< Default constructor. See clear() for the postconditions.

  ImageData(const std::uint8_t* data,
            PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint16_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown) noexcept;

  ~ImageData() = default;  ///< Destructor.

  /// Copy constructor.
  ImageData(const ImageData<ImageDataStorage::Constant>&) = default;
  /// Copy assignment operator
  ImageData<ImageDataStorage::Constant>& operator=(const ImageData<ImageDataStorage::Constant>&) = default;
  /// Move constructor.
  ImageData(ImageData<ImageDataStorage::Constant>&&) noexcept = default;
  /// Move assignment operator
  ImageData<ImageDataStorage::Constant>& operator=(ImageData<ImageDataStorage::Constant>&&) noexcept = default;
};


/** \brief Explicit specialization of `ImageData` for modifiable image data.
 *
 * Can only point to both owned as well as non-owned memory.
 */
template <>
class ImageData<ImageDataStorage::Modifiable> : public ImageDataBase<std::uint8_t*>
{
public:
  ImageData() = default;  ///< Default constructor. See clear() for the postconditions.

  ImageData(PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint16_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown);

  ImageData(PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint16_t nr_bytes_per_channel,
            ImageRowAlignment row_alignment_bytes,
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown);

  ImageData(std::uint8_t* data,
            PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint16_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown) noexcept;

  ImageData(MemoryBlock<AlignedNewAllocator>&& data,
            PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint16_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown) noexcept;

  ~ImageData();

  ImageData(const ImageData<ImageDataStorage::Modifiable>&);
  ImageData<ImageDataStorage::Modifiable>& operator=(const ImageData<ImageDataStorage::Modifiable>&);
  ImageData(ImageData<ImageDataStorage::Modifiable>&&) noexcept;
  ImageData<ImageDataStorage::Modifiable>& operator=(ImageData<ImageDataStorage::Modifiable>&&) noexcept;

  bool is_view() const noexcept;

  void clear() noexcept;

  void allocate(PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown,
                bool shrink_to_fit = true,
                bool force_allocation = false,
                bool allow_view_reallocation = true);

  void allocate(PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                ImageRowAlignment row_alignment_bytes,
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown,
                bool shrink_to_fit = true,
                bool force_allocation = false,
                bool allow_view_reallocation = true);

  void set_view(std::uint8_t* data,
                PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown);

  void set_data(MemoryBlock<AlignedNewAllocator>&& data,
                PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown);

  std::uint8_t* byte_ptr() noexcept;
  const std::uint8_t* byte_ptr() const noexcept;

  std::uint8_t* byte_ptr(PixelIndex y) noexcept;
  const std::uint8_t* byte_ptr(PixelIndex y) const noexcept;

  std::uint8_t* byte_ptr(PixelIndex x, PixelIndex y) noexcept;
  const std::uint8_t* byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

private:
  bool owns_memory_ = false;

  constexpr static std::size_t default_base_alignment_ = 16;

  void allocate(PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint16_t nr_bytes_per_channel,
                Stride stride_bytes,
                std::size_t base_alignment_bytes,
                PixelFormat pixel_format,
                SampleFormat sample_format,
                bool shrink_to_fit,
                bool force_allocation,
                bool allow_view_reallocation);
  void allocate_bytes(std::size_t nr_bytes, std::size_t alignment);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();

  MemoryBlock<AlignedNewAllocator> relinquish_data_ownership();

  template <typename PixelType>
  friend Image<PixelType> to_image(ImageData&&);
};


// ----------
// Implementation:


// ImageData<ImageDataStorage::Constant>:

/** \brief Constructs image data (a view onto non-owned memory) with the specified parameters.
 *
 * Effectively calls `set_view(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
 * output_sample_format);`.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * @param data Pointer to the existing image data.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The desired row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
inline ImageData<ImageDataStorage::Constant>::ImageData(const std::uint8_t* data,
                                                        PixelLength width,
                                                        PixelLength height,
                                                        std::uint16_t nr_channels,
                                                        std::uint16_t nr_bytes_per_channel,
                                                        Stride stride_bytes,
                                                        PixelFormat pixel_format,
                                                        SampleFormat sample_format) noexcept
{
  set_view(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_format);
}


// ImageData<ImageDataStorage::Modifiable>:

/** \brief Constructs image data (owned memory) with the specified parameters.
 *
 * Effectively calls `allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
 * output_sample_format)`.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * The image (row) data is not guaranteed to be aligned in any way.
 *
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The desired row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
inline ImageData<ImageDataStorage::Modifiable>::ImageData(PixelLength width,
                                                          PixelLength height,
                                                          std::uint16_t nr_channels,
                                                          std::uint16_t nr_bytes_per_channel,
                                                          Stride stride_bytes,
                                                          PixelFormat pixel_format,
                                                          SampleFormat sample_format)
{
  constexpr auto base_alignment_bytes = ImageData<ImageDataStorage::Modifiable>::default_base_alignment_;
  constexpr bool shrink_to_fit = true;
  constexpr bool force_allocation = false;
  constexpr bool allow_view_reallocation = true;
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, base_alignment_bytes, pixel_format,
           sample_format, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Constructs image data (owned memory) with the specified parameters.
 *
 * The row stride (in bytes) is chosen to be the smallest value that satisfies the row alignment requirements.
 *
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param row_alignment_bytes The row alignment in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
inline ImageData<ImageDataStorage::Modifiable>::ImageData(PixelLength width,
                                                          PixelLength height,
                                                          std::uint16_t nr_channels,
                                                          std::uint16_t nr_bytes_per_channel,
                                                          ImageRowAlignment row_alignment_bytes,
                                                          PixelFormat pixel_format,
                                                          SampleFormat sample_format)
{
  const auto row_bytes = nr_bytes_per_channel * nr_channels * width;
  const auto stride_bytes = detail::compute_stride_bytes(row_bytes, row_alignment_bytes);
  constexpr bool shrink_to_fit = true;
  constexpr bool force_allocation = false;
  constexpr bool allow_view_reallocation = true;
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, row_alignment_bytes, pixel_format,
           sample_format, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Constructs image data (a view onto non-owned memory) with the specified parameters.
 *
 * Effectively calls `set_view(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
 * output_sample_format);`.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * @param data Pointer to the existing image data.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The desired row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */

inline ImageData<ImageDataStorage::Modifiable>::ImageData(std::uint8_t* data,
                                                          PixelLength width,
                                                          PixelLength height,
                                                          std::uint16_t nr_channels,
                                                          std::uint16_t nr_bytes_per_channel,
                                                          Stride stride_bytes,
                                                          PixelFormat pixel_format,
                                                          SampleFormat sample_format) noexcept
{
  set_view(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_format);
}

/** \brief Constructs image data from existing memory (which will be owned) with the specified parameters.
 *
 * Effectively calls `set_data(std::move(data), width, height, nr_channels, nr_bytes_per_channel, stride_bytes,
 * pixel_format, output_sample_format);`.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * @param data A `MemoryBlock<NewAllocator>` with the existing data.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The desired row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
inline ImageData<ImageDataStorage::Modifiable>::ImageData(MemoryBlock<AlignedNewAllocator>&& data,
                                                          PixelLength width,
                                                          PixelLength height,
                                                          std::uint16_t nr_channels,
                                                          std::uint16_t nr_bytes_per_channel,
                                                          Stride stride_bytes,
                                                          PixelFormat pixel_format,
                                                          SampleFormat sample_format) noexcept
{
  set_data(std::move(data), width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
           sample_format);
}

/** \brief Destructor.
 *
 * Owned data will be deallocated at destruction time.
 */
inline ImageData<ImageDataStorage::Modifiable>::~ImageData()
{
  deallocate_bytes_if_owned();
}

/** \brief Copy constructor.
 *
 * Constructs an image data instance from the supplied image data.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @param other The source image.
 */
inline ImageData<ImageDataStorage::Modifiable>::ImageData(const ImageData<ImageDataStorage::Modifiable>& other)
    : ImageDataBase<std::uint8_t*>::ImageDataBase(other)
{
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    const auto nr_bytes = stride_bytes_ * height_;
    allocate_bytes(nr_bytes, detail::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                         other.stride_bytes()));
  }
}

/** \brief Copy assignment operator
 *
 * Assigns another image instance.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @param other The image to assign from.
 * @return A reference to this image.
 */
inline ImageData<ImageDataStorage::Modifiable>& ImageData<ImageDataStorage::Modifiable>::operator=(
    const ImageData<ImageDataStorage::Modifiable>& other)
{
  deallocate_bytes_if_owned();

  data_ = other.data_;
  width_ = other.width_;
  height_ = other.height_;
  stride_bytes_ = other.stride_bytes_;
  nr_channels_ = other.nr_channels_;
  nr_bytes_per_channel_ = other.nr_bytes_per_channel_;
  pixel_format_ = other.pixel_format_;
  sample_format_ = other.sample_format_;
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    const auto nr_bytes = stride_bytes_ * height_;
    allocate_bytes(nr_bytes, detail::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                         other.stride_bytes()));
  }

  return *this;
}

/** \brief Move constructor.
 *
 * @param other The image to move from.
 */
inline ImageData<ImageDataStorage::Modifiable>::ImageData(ImageData<ImageDataStorage::Modifiable>&& other) noexcept
{
  data_ = other.data_;
  width_ = other.width_;
  height_ = other.height_;
  stride_bytes_ = other.stride_bytes_;
  nr_channels_ = other.nr_channels_;
  nr_bytes_per_channel_ = other.nr_bytes_per_channel_;
  pixel_format_ = other.pixel_format_;
  sample_format_ = other.sample_format_;
  owns_memory_ = other.owns_memory_;

  other.reset();
}

/**\brief Move assignment operator.
 *
 * @param other The image to move assign from.
 * @return A reference to this image.
 */
inline ImageData<ImageDataStorage::Modifiable>& ImageData<ImageDataStorage::Modifiable>::operator=(
    ImageData<ImageDataStorage::Modifiable>&& other) noexcept
{
  deallocate_bytes_if_owned();

  data_ = other.data_;
  width_ = other.width_;
  height_ = other.height_;
  stride_bytes_ = other.stride_bytes_;
  nr_channels_ = other.nr_channels_;
  nr_bytes_per_channel_ = other.nr_bytes_per_channel_;
  pixel_format_ = other.pixel_format_;
  sample_format_ = other.sample_format_;
  owns_memory_ = other.owns_memory_;

  other.reset();
  return *this;
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
inline bool ImageData<ImageDataStorage::Modifiable>::is_view() const noexcept
{
  return !owns_memory_;
}

/** \brief Resets the image instance by clearing the image data and resetting the internal state to the state after
 * default construction.
 *
 * Postconditions: `data() == nullptr && width() == 0 && height() == 0 && stride_bytes() == 0 && nr_channels() == 0 &&
 * nr_bytes_per_channel() == 0 && pixel_format() == PixelFormat::Unknown && output_sample_format() ==
 * SampleType::Unknown && is_empty() && !is_valid() && !is_view()`.
 */
inline void ImageData<ImageDataStorage::Modifiable>::clear() noexcept
{
  deallocate_bytes_if_owned();
  reset();
}

/** \brief Allocates memory for an image with the specified parameters.
 *
 * Allocates `stride_bytes * height` bytes of memory to represent an image with the respective width, height, and number
 * of channels per pixel element.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= nr_bytes_per_channel * nr_channels * width)`.
 *
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 * @param shrink_to_fit If true, reallocate if it results in less memory usage; otherwise allow excess memory to stay
 * allocated
 * @param force_allocation If true, always force a reallocation. Overrides `allow_view_reallocation == false`.
 * @param allow_view_reallocation If true, allow allocation from `is_view() == true`. If false, and the existing image
 * is a view, a `std::runtime_error` exception will be thrown (respecting the strong exception guarantee).
 */
inline void ImageData<ImageDataStorage::Modifiable>::allocate(PixelLength width,
                                                              PixelLength height,
                                                              std::uint16_t nr_channels,
                                                              std::uint16_t nr_bytes_per_channel,
                                                              Stride stride_bytes,
                                                              PixelFormat pixel_format,
                                                              SampleFormat sample_format,
                                                              bool shrink_to_fit,
                                                              bool force_allocation,
                                                              bool allow_view_reallocation)
{
  constexpr auto base_alignment_bytes = ImageData<ImageDataStorage::Modifiable>::default_base_alignment_;
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, base_alignment_bytes, pixel_format,
           sample_format, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Allocates memory for an image with the specified parameters.
 *
 * Allocates memory to represent an image with the respective width, height, and number of channels per pixel element,
 * while satisfying the specified row alignment.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= nr_bytes_per_channel * nr_channels * width)`.
 *
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param row_alignment_bytes The desired row alignment in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 * @param shrink_to_fit If true, reallocate if it results in less memory usage; otherwise allow excess memory to stay
 * allocated
 * @param force_allocation If true, always force a reallocation. Overrides `allow_view_reallocation == false`.
 * @param allow_view_reallocation If true, allow allocation from `is_view() == true`. If false, and the existing image
 * is a view, a `std::runtime_error` exception will be thrown (respecting the strong exception guarantee).
 */
inline void ImageData<ImageDataStorage::Modifiable>::allocate(PixelLength width,
                                                              PixelLength height,
                                                              std::uint16_t nr_channels,
                                                              std::uint16_t nr_bytes_per_channel,
                                                              ImageRowAlignment row_alignment_bytes,
                                                              PixelFormat pixel_format,
                                                              SampleFormat sample_format,
                                                              bool shrink_to_fit,
                                                              bool force_allocation,
                                                              bool allow_view_reallocation)
{
  const auto row_bytes = nr_bytes_per_channel * nr_channels * width;
  const auto stride_bytes = detail::compute_stride_bytes(row_bytes, row_alignment_bytes);
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, row_alignment_bytes, pixel_format,
           sample_format, shrink_to_fit, force_allocation, allow_view_reallocation);
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
inline void ImageData<ImageDataStorage::Modifiable>::set_view(std::uint8_t* data,
                                                              PixelLength width,
                                                              PixelLength height,
                                                              std::uint16_t nr_channels,
                                                              std::uint16_t nr_bytes_per_channel,
                                                              Stride stride_bytes,
                                                              PixelFormat pixel_format,
                                                              SampleFormat sample_format)
{
  stride_bytes = std::max(stride_bytes, Stride(nr_bytes_per_channel * nr_channels * width));

  deallocate_bytes_if_owned();
  data_ = data;
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_format_ = sample_format;
  owns_memory_ = false;
}

/** \brief Sets the image data to the provided memory block, which will be owned by the `ImageData` instance.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * Precondition: `data.size() >= stride_bytes * height`.
 *
 * Postcondition: `!is_view()`.
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
inline void ImageData<ImageDataStorage::Modifiable>::set_data(MemoryBlock<AlignedNewAllocator>&& data,
                                                              PixelLength width,
                                                              PixelLength height,
                                                              std::uint16_t nr_channels,
                                                              std::uint16_t nr_bytes_per_channel,
                                                              Stride stride_bytes,
                                                              PixelFormat pixel_format,
                                                              SampleFormat sample_format)
{
  stride_bytes = std::max(stride_bytes, Stride(nr_bytes_per_channel * nr_channels * width));
  SELENE_ASSERT(data.size() >= stride_bytes * height);

  deallocate_bytes_if_owned();
  data_ = data.transfer_data();
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_format_ = sample_format;
  owns_memory_ = true;
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @return Pointer to the first image data byte.
 */
inline std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr() noexcept
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @return Constant pointer to the first image data byte.
 */
inline const std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr() const noexcept
{
  return data_;
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
inline std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr(PixelIndex y) noexcept
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
inline const std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr(PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
inline std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr(PixelIndex x, PixelIndex y) noexcept
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column
 * `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
inline const std::uint8_t* ImageData<ImageDataStorage::Modifiable>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(x, y);
}

inline void ImageData<ImageDataStorage::Modifiable>::allocate(PixelLength width,
                                                              PixelLength height,
                                                              std::uint16_t nr_channels,
                                                              std::uint16_t nr_bytes_per_channel,
                                                              Stride stride_bytes,
                                                              std::size_t base_alignment_bytes,
                                                              PixelFormat pixel_format,
                                                              SampleFormat sample_format,
                                                              bool shrink_to_fit,
                                                              bool force_allocation,
                                                              bool allow_view_reallocation)
{
  stride_bytes = std::max(stride_bytes, Stride(nr_bytes_per_channel * nr_channels * width));
  const auto nr_bytes_to_allocate = stride_bytes * height;
  const auto nr_currently_allocated_bytes = total_bytes();

  auto commit_new_geometry = [=]() {
    width_ = width;
    height_ = height;
    stride_bytes_ = stride_bytes;
    nr_channels_ = nr_channels;
    nr_bytes_per_channel_ = nr_bytes_per_channel;
    pixel_format_ = pixel_format;
    sample_format_ = sample_format;
  };

  const auto bytes_match = shrink_to_fit ? (nr_bytes_to_allocate == nr_currently_allocated_bytes)
                                         : (nr_bytes_to_allocate <= nr_currently_allocated_bytes);
  if (!force_allocation && bytes_match && owns_memory_)
  {
    commit_new_geometry();
    return;
  }

  if (!owns_memory_ && !allow_view_reallocation && !force_allocation)
  {
    throw std::runtime_error("Cannot allocate from image that is a view to external memory.");
  }

  commit_new_geometry();

  deallocate_bytes_if_owned();
  owns_memory_ = true;
  allocate_bytes(nr_bytes_to_allocate, base_alignment_bytes);
}

inline void ImageData<ImageDataStorage::Modifiable>::allocate_bytes(std::size_t nr_bytes, std::size_t alignment)
{
  SELENE_ASSERT(owns_memory_);
  auto memory = AlignedNewAllocator::allocate(nr_bytes, alignment);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

inline void ImageData<ImageDataStorage::Modifiable>::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);
  AlignedNewAllocator::deallocate(data_);
}

inline void ImageData<ImageDataStorage::Modifiable>::deallocate_bytes_if_owned()
{
  if (owns_memory_)
  {
    deallocate_bytes();
  }
}

inline void ImageData<ImageDataStorage::Modifiable>::reset()
{
  ImageDataBase<std::uint8_t*>::reset();
  owns_memory_ = false;
}

inline MemoryBlock<AlignedNewAllocator> ImageData<ImageDataStorage::Modifiable>::relinquish_data_ownership()
{
  SELENE_FORCED_ASSERT(owns_memory_);
  const auto ptr = data_;
  const auto len = total_bytes();

  owns_memory_ = false;
  clear();
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, len);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_DATA_HPP

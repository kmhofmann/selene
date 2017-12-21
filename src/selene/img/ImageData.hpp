// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_HPP
#define SELENE_IMG_IMAGE_DATA_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/PixelFormat.hpp>
#include <selene/img/Types.hpp>

#include <cstdint>
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
 * Optionally, an image can have be tagged with a particular `PixelFormat` or a partiular `SampleType`.
 * This is mostly a semantic tag and has little influence on the data content.
 *
 * The memory of an `ImageData` instance may either be owned or non-owned; in the latter case, the instance is a "view"
 * on image data.
 */
class ImageData
{
public:
  ImageData() = default;  ///< Default constructor. See clear() for the postconditions.

  ImageData(PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown);

  ImageData(std::uint8_t* data,
            PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown);

  ImageData(MemoryBlock<NewAllocator>&& data,
            PixelLength width,
            PixelLength height,
            std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel,
            Stride stride_bytes = Stride{0},
            PixelFormat pixel_format = PixelFormat::Unknown,
            SampleFormat sample_format = SampleFormat::Unknown);
  ~ImageData();

  ImageData(const ImageData&);
  ImageData& operator=(const ImageData&);
  ImageData(ImageData&&) noexcept;
  ImageData& operator=(ImageData&&) noexcept;

  PixelLength width() const;
  PixelLength height() const;
  std::uint16_t nr_channels() const;
  std::uint8_t nr_bytes_per_channel() const;
  Stride stride_bytes() const;
  std::size_t row_bytes() const;
  std::size_t total_bytes() const;
  PixelFormat pixel_format() const;
  SampleFormat sample_format() const;
  bool is_packed() const;
  bool is_view() const;
  bool is_empty() const;
  bool is_valid() const;

  void clear();

  void allocate(PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown,
                bool shrink_to_fit = true,
                bool force_allocation = false,
                bool allow_view_reallocation = true);

  void set_view(std::uint8_t* data,
                PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown);

  void set_data(MemoryBlock<NewAllocator>&& data,
                PixelLength width,
                PixelLength height,
                std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0},
                PixelFormat pixel_format = PixelFormat::Unknown,
                SampleFormat sample_format = SampleFormat::Unknown);

  std::uint8_t* byte_ptr();
  const std::uint8_t* byte_ptr() const;

  std::uint8_t* byte_ptr(PixelIndex y);
  const std::uint8_t* byte_ptr(PixelIndex y) const;

  std::uint8_t* byte_ptr(PixelIndex x, PixelIndex y);
  const std::uint8_t* byte_ptr(PixelIndex x, PixelIndex y) const;

private:
  std::uint8_t* data_ = nullptr;
  PixelLength width_ = 0_px;
  PixelLength height_ = 0_px;
  Stride stride_bytes_ = 0_b;
  std::uint16_t nr_channels_ = 0;
  std::uint8_t nr_bytes_per_channel_ = 0;
  PixelFormat pixel_format_ = PixelFormat::Unknown;
  SampleFormat sample_format_ = SampleFormat::Unknown;
  bool owns_memory_ = false;

  void allocate_bytes(std::size_t nr_bytes);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();
  Bytes compute_data_offset(PixelIndex y) const;
  Bytes compute_data_offset(PixelIndex x, PixelIndex y) const;

  MemoryBlock<NewAllocator> relinquish_data_ownership();

  template <typename PixelType>
  friend Image<PixelType> to_image(ImageData&&);
};

/** \brief Constructs image data (owned memory) with the specified parameters.
 *
 * Effectively calls `allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
 * output_sample_format)`.
 *
 * The row stride (in bytes) is chosen to be at least `nr_bytes_per_channel * nr_channels * width`, or the supplied
 * value.
 *
 * @param width Desired image width.
 * @param height Desired image height.
 * @param nr_channels The number of channels per pixel element.
 * @param nr_bytes_per_channel The number of bytes stored per channel.
 * @param stride_bytes The desired row stride in bytes.
 * @param pixel_format The pixel format (semantic tag).
 * @param sample_format The sample format (semantic tag).
 */
inline ImageData::ImageData(PixelLength width,
                            PixelLength height,
                            std::uint16_t nr_channels,
                            std::uint8_t nr_bytes_per_channel,
                            Stride stride_bytes,
                            PixelFormat pixel_format,
                            SampleFormat sample_format)
{
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_format);
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
inline ImageData::ImageData(std::uint8_t* data,
                            PixelLength width,
                            PixelLength height,
                            std::uint16_t nr_channels,
                            std::uint8_t nr_bytes_per_channel,
                            Stride stride_bytes,
                            PixelFormat pixel_format,
                            SampleFormat sample_format)
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
inline ImageData::ImageData(MemoryBlock<NewAllocator>&& data,
                            PixelLength width,
                            PixelLength height,
                            std::uint16_t nr_channels,
                            std::uint8_t nr_bytes_per_channel,
                            Stride stride_bytes,
                            PixelFormat pixel_format,
                            SampleFormat sample_format)
{
  set_data(std::move(data), width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
           sample_format);
}

/** \brief Destructor.
 *
 * Owned data will be deallocated at destruction time.
 */
inline ImageData::~ImageData()
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
inline ImageData::ImageData(const ImageData& other)
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

  if (other.owns_memory_)
  {
    const auto nr_bytes = stride_bytes_ * height_;
    allocate_bytes(nr_bytes);
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
inline ImageData& ImageData::operator=(const ImageData& other)
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
    allocate_bytes(nr_bytes);
  }

  return *this;
}

/** \brief Move constructor.
 *
 * @param other The image to move from.
 */
inline ImageData::ImageData(ImageData&& other) noexcept
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
inline ImageData& ImageData::operator=(ImageData&& other) noexcept
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

/** \brief Returns the image width.
 *
 * @return Width of the image in pixels.
 */
inline PixelLength ImageData::width() const
{
  return width_;
}

/** \brief Returns the image height.
 *
 * @return Height of the image in pixels.
 */
inline PixelLength ImageData::height() const
{
  return height_;
}

/** \brief Returns the number of image channels.
 *
 * @return Number of image channels.
 */
inline std::uint16_t ImageData::nr_channels() const
{
  return nr_channels_;
}

/** \brief Returns the number of bytes stored for each sample, per image channel.
 *
 * @return Number of bytes stored for each sample, per image channel.
 */
inline std::uint8_t ImageData::nr_bytes_per_channel() const
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
inline Stride ImageData::stride_bytes() const
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
inline std::size_t ImageData::row_bytes() const
{
  return width_ * nr_channels_ * nr_bytes_per_channel_;
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @return Number of bytes occupied by the image data in memory.
 */
inline std::size_t ImageData::total_bytes() const
{
  return stride_bytes_ * height_;
}

/** \brief Returns the pixel format (semantic tag).
 *
 * @return The pixel format.
 */
inline PixelFormat ImageData::pixel_format() const
{
  return pixel_format_;
}

/** \brief Returns the sample format (semantic tag).
 *
 * @return The sample format.
 */
inline SampleFormat ImageData::sample_format() const
{
  return sample_format_;
}

/** \brief Returns whether the image data is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * nr_channels() * nr_bytes_per_channel())`.
 *
 * @return True, if the image data stored packed; false otherwise.
 */
inline bool ImageData::is_packed() const
{
  return stride_bytes_ == nr_bytes_per_channel_ * nr_channels_ * width_;
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
inline bool ImageData::is_view() const
{
  return !owns_memory_;
}

/** \brief Returns whether the image is empty.
 *
 * An image is considered empty if its internal data pointer points to `nullptr`, `width() == 0`, `height() == 0`,
 * `nr_channels() == 0`, `nr_bytes_per_channel() == 0`, or any combination of these.
 *
 * @return True, if the image is empty; false if it is non-empty.
 */
inline bool ImageData::is_empty() const
{
  return data_ == nullptr || width_ == 0 || height_ == 0 || nr_channels_ == 0 || nr_bytes_per_channel_ == 0;
}

/** \brief Returns whether the instance represents a valid image.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @return True, if the image is valid; false otherwise.
 */
inline bool ImageData::is_valid() const
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
inline void ImageData::clear()
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

inline void ImageData::allocate(PixelLength width,
                                PixelLength height,
                                std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel,
                                Stride stride_bytes,
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
  allocate_bytes(nr_bytes_to_allocate);
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
inline void ImageData::set_view(std::uint8_t* data,
                                PixelLength width,
                                PixelLength height,
                                std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel,
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
inline void ImageData::set_data(MemoryBlock<NewAllocator>&& data,
                                PixelLength width,
                                PixelLength height,
                                std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel,
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
inline const std::uint8_t* ImageData::byte_ptr() const
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @return Constant pointer to the first image data byte.
 */
inline std::uint8_t* ImageData::byte_ptr()
{
  return data_;
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
inline std::uint8_t* ImageData::byte_ptr(PixelIndex y)
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
inline const std::uint8_t* ImageData::byte_ptr(PixelIndex y) const
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
inline std::uint8_t* ImageData::byte_ptr(PixelIndex x, PixelIndex y)
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
inline const std::uint8_t* ImageData::byte_ptr(PixelIndex x, PixelIndex y) const
{
  return data_ + compute_data_offset(x, y);
}

inline void ImageData::allocate_bytes(std::size_t nr_bytes)
{
  SELENE_ASSERT(owns_memory_);
  auto memory = NewAllocator::allocate(nr_bytes);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

inline void ImageData::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);
  NewAllocator::deallocate(data_);
}

inline void ImageData::deallocate_bytes_if_owned()
{
  if (owns_memory_)
  {
    deallocate_bytes();
  }
}

inline void ImageData::reset()
{
  // reset to default constructed state
  data_ = nullptr;
  width_ = PixelLength{0};
  height_ = PixelLength{0};
  stride_bytes_ = Stride{0};
  nr_channels_ = std::uint16_t{0};
  nr_bytes_per_channel_ = std::uint8_t{0};
  pixel_format_ = PixelFormat::Unknown;
  sample_format_ = SampleFormat::Unknown;
  owns_memory_ = false;
}

inline Bytes ImageData::compute_data_offset(PixelIndex y) const
{
  return Bytes(stride_bytes_ * y);
}

inline Bytes ImageData::compute_data_offset(PixelIndex x, PixelIndex y) const
{
  return Bytes(stride_bytes_ * y + nr_bytes_per_channel_ * nr_channels_ * x);
}

inline MemoryBlock<NewAllocator> ImageData::relinquish_data_ownership()
{
  SELENE_FORCED_ASSERT(owns_memory_);
  const auto ptr = data_;
  const auto len = total_bytes();

  owns_memory_ = false;
  clear();
  return construct_memory_block_from_existing_memory<NewAllocator>(ptr, len);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_DATA_HPP

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

namespace selene {
namespace img {

template <typename T> class Image;

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
  ImageData() = default;
  ImageData(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
            PixelFormat pixel_format = PixelFormat::Unknown, SampleType sample_type = SampleType::Unknown);
  ImageData(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
            Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
            SampleType sample_type = SampleType::Unknown);
  ImageData(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format = PixelFormat::Unknown,
            SampleType sample_type = SampleType::Unknown);
  ImageData(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
            SampleType sample_type = SampleType::Unknown);
  ImageData(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format = PixelFormat::Unknown,
            SampleType sample_type = SampleType::Unknown);
  ImageData(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
            std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
            SampleType sample_type = SampleType::Unknown);
  ~ImageData();

  ImageData(const ImageData&);
  ImageData& operator=(const ImageData&);
  ImageData(ImageData&&) noexcept;
  ImageData& operator=(ImageData&&) noexcept;

  Length width() const;
  Length height() const;
  std::uint16_t nr_channels() const;
  std::uint8_t nr_bytes_per_channel() const;
  Stride stride_bytes() const;
  std::size_t total_bytes() const;
  PixelFormat pixel_format() const;
  SampleType sample_type() const;
  bool is_packed() const;
  bool is_view() const;
  bool is_empty() const;
  bool is_valid() const;

  void clear();

  void allocate(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
                PixelFormat pixel_format = PixelFormat::Unknown, SampleType sample_type = SampleType::Unknown);
  void allocate(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
                Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
                SampleType sample_type = SampleType::Unknown);

  void set_view(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format = PixelFormat::Unknown,
                SampleType sample_type = SampleType::Unknown);
  void set_view(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
                SampleType sample_type = SampleType::Unknown);

  void set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format = PixelFormat::Unknown,
                SampleType sample_type = SampleType::Unknown);
  void set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
                std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format = PixelFormat::Unknown,
                SampleType sample_type = SampleType::Unknown);

  std::uint8_t* byte_ptr();
  const std::uint8_t* byte_ptr() const;

  std::uint8_t* byte_ptr(Index y);
  const std::uint8_t* byte_ptr(Index y) const;

  std::uint8_t* byte_ptr(Index x, Index y);
  const std::uint8_t* byte_ptr(Index x, Index y) const;

private:
  std::uint8_t* data_ = nullptr;
  Length width_ = 0;
  Length height_ = 0;
  Stride stride_bytes_ = 0;
  std::uint16_t nr_channels_ = 0;
  std::uint8_t nr_bytes_per_channel_ = 0;
  PixelFormat pixel_format_ = PixelFormat::Unknown;
  SampleType sample_type_ = SampleType::Unknown;
  bool owns_memory_ = false;

  void allocate_bytes(std::size_t nr_bytes);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();
  std::uint32_t compute_data_offset(Index y) const;
  std::uint32_t compute_data_offset(Index x, Index y) const;

  MemoryBlock<NewAllocator> relinquish_data_ownership();

  template <typename PixelType> friend Image<PixelType> to_image(ImageData&&);
};

inline ImageData::ImageData(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
                            PixelFormat pixel_format, SampleType sample_type)
{
  allocate(width, height, nr_channels, nr_bytes_per_channel, pixel_format, sample_type);
}

inline ImageData::ImageData(Length width, Length height, std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel,
                            Stride stride_bytes, PixelFormat pixel_format, SampleType sample_type)
{
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type);
}

inline ImageData::ImageData(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                            std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format, SampleType sample_type)
{
  set_view(data, width, height, nr_channels, nr_bytes_per_channel, pixel_format, sample_type);
}

inline ImageData::ImageData(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                            std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format,
                            SampleType sample_type)
{
  set_view(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type);
}

inline ImageData::ImageData(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
    std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format, SampleType sample_type)
{
  set_data(std::move(data), width, height, nr_channels, nr_bytes_per_channel, pixel_format, sample_type);
}

inline ImageData::ImageData(MemoryBlock<NewAllocator>&& data, Length width, Length height, std::uint16_t nr_channels,
    std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format, SampleType sample_type)
{
  set_data(std::move(data), width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type);
}

inline ImageData::~ImageData()
{
  deallocate_bytes_if_owned();
}

inline ImageData::ImageData(const ImageData& other)
{
  data_ = other.data_;
  width_ = other.width_;
  height_ = other.height_;
  stride_bytes_ = other.stride_bytes_;
  nr_channels_ = other.nr_channels_;
  nr_bytes_per_channel_ = other.nr_bytes_per_channel_;
  pixel_format_ = other.pixel_format_;
  sample_type_ = other.sample_type_;
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    const auto nr_bytes = stride_bytes_ * height_;
    allocate_bytes(nr_bytes);
  }
}

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
  sample_type_ = other.sample_type_;
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    const auto nr_bytes = stride_bytes_ * height_;
    allocate_bytes(nr_bytes);
  }

  return *this;
}

inline ImageData::ImageData(ImageData&& other) noexcept
{
  data_ = other.data_;
  width_ = other.width_;
  height_ = other.height_;
  stride_bytes_ = other.stride_bytes_;
  nr_channels_ = other.nr_channels_;
  nr_bytes_per_channel_ = other.nr_bytes_per_channel_;
  pixel_format_ = other.pixel_format_;
  sample_type_ = other.sample_type_;
  owns_memory_ = other.owns_memory_;

  other.reset();
}

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
  sample_type_ = other.sample_type_;
  owns_memory_ = other.owns_memory_;

  other.reset();
  return *this;
}

inline Length ImageData::width() const
{
  return width_;
}

inline Length ImageData::height() const
{
  return height_;
}

inline std::uint16_t ImageData::nr_channels() const
{
  return nr_channels_;
}

inline std::uint8_t ImageData::nr_bytes_per_channel() const
{
  return nr_bytes_per_channel_;
}

inline Stride ImageData::stride_bytes() const
{
  return stride_bytes_;
}

inline std::size_t ImageData::total_bytes() const
{
  return stride_bytes_ * height_;
}

inline PixelFormat ImageData::pixel_format() const
{
  return pixel_format_;
}

inline SampleType ImageData::sample_type() const
{
  return sample_type_;
}

inline bool ImageData::is_packed() const
{
  return stride_bytes_ == nr_bytes_per_channel_ * nr_channels_ * width_;
}

inline bool ImageData::is_view() const
{
  return !owns_memory_;
}

inline bool ImageData::is_empty() const
{
  return data_ == nullptr || width_ == 0 || height_ == 0 || nr_channels_ == 0 || nr_bytes_per_channel_ == 0;
}

inline bool ImageData::is_valid() const
{
  return !is_empty();
}

inline void ImageData::clear()
{
  deallocate_bytes_if_owned();
  reset();
}

inline void ImageData::allocate(Length width, Length height, std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format, SampleType sample_type)
{
  Stride stride_bytes = nr_bytes_per_channel * nr_channels * width;
  allocate(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type);
}

inline void ImageData::allocate(Length width, Length height, std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format,
                                SampleType sample_type)
{
  if (width_ == width && height_ == height && stride_bytes_ == stride_bytes &&
      nr_channels_ == nr_channels && nr_bytes_per_channel_ == nr_bytes_per_channel &&
      pixel_format_ == pixel_format && sample_type_ == sample_type)
  {
    return;
  }

  deallocate_bytes_if_owned();
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_type_ = sample_type;
  owns_memory_ = true;

  const auto nr_bytes = stride_bytes_ * height_;
  allocate_bytes(nr_bytes);
}

inline void ImageData::set_view(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format, SampleType sample_type)
{
  deallocate_bytes_if_owned();
  data_ = data;
  width_ = width;
  height_ = height;
  stride_bytes_ = nr_bytes_per_channel * nr_channels * width;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_type_ = sample_type;
  owns_memory_ = false;
}

inline void ImageData::set_view(std::uint8_t* data, Length width, Length height, std::uint16_t nr_channels,
                                std::uint8_t nr_bytes_per_channel, Stride stride_bytes, PixelFormat pixel_format,
                                SampleType sample_type)
{
  deallocate_bytes_if_owned();
  data_ = data;
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_type_ = sample_type;
  owns_memory_ = false;
}

inline void ImageData::set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height,
                                std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel, PixelFormat pixel_format,
                                SampleType sample_type)
{
  deallocate_bytes_if_owned();
  data_ = data.transfer_data();
  width_ = width;
  height_ = height;
  stride_bytes_ = nr_bytes_per_channel * nr_channels * width;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_type_ = sample_type;
  owns_memory_ = true;
}

inline void ImageData::set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height,
                                std::uint16_t nr_channels, std::uint8_t nr_bytes_per_channel, Stride stride_bytes,
                                PixelFormat pixel_format, SampleType sample_type)
{
  deallocate_bytes_if_owned();
  data_ = data.transfer_data();
  width_ = width;
  height_ = height;
  stride_bytes_ = stride_bytes;
  nr_channels_ = nr_channels;
  nr_bytes_per_channel_ = nr_bytes_per_channel;
  pixel_format_ = pixel_format;
  sample_type_ = sample_type;
  owns_memory_ = true;
}

inline const std::uint8_t* ImageData::byte_ptr() const
{
  return data_;
}

inline std::uint8_t* ImageData::byte_ptr()
{
  return data_;
}

inline std::uint8_t* ImageData::byte_ptr(Index y)
{
  return data_ + compute_data_offset(y);
}

inline const std::uint8_t* ImageData::byte_ptr(Index y) const
{
  return data_ + compute_data_offset(y);
}

inline std::uint8_t* ImageData::byte_ptr(Index x, Index y)
{
  return data_ + compute_data_offset(x, y);
}

inline const std::uint8_t* ImageData::byte_ptr(Index x, Index y) const
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
  width_ = Length{0};
  height_ = Length{0};
  stride_bytes_ = Stride{0};
  nr_channels_ = std::uint16_t{0};
  nr_bytes_per_channel_ = std::uint8_t{0};
  pixel_format_ = PixelFormat::Unknown;
  sample_type_ = SampleType::Unknown;
  owns_memory_ = false;
}

inline std::uint32_t ImageData::compute_data_offset(Index y) const
{
  return stride_bytes_ * y;
}

inline std::uint32_t ImageData::compute_data_offset(Index x, Index y) const
{
  return stride_bytes_ * y + nr_bytes_per_channel_ * nr_channels_ * x;
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

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IMAGE_DATA_HPP
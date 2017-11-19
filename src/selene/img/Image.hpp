// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_HPP
#define SELENE_IMG_IMAGE_HPP

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>

#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>
#include <selene/img/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>

namespace selene {
namespace img {

class ImageData;
template <typename T> class Image;
template <typename T> void clone(const Image<T>& src, Image<T>& dst);

template <typename T>
class Image
{
public:
  Image();
  Image(Length width, Length height);
  Image(Length width, Length height, T value);
  Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes, bool take_ownership = false);

  Image(const Image<T>& other);
  Image<T>& operator=(const Image<T>& other);

  Image(Image<T>&& other) noexcept;
  Image<T>& operator=(Image<T>&& other) noexcept;

  ~Image();

  Length width() const;
  Length height() const;
  Stride stride_bytes() const;
  std::size_t total_bytes() const;
  bool is_packed() const;
  bool is_view() const;
  bool is_empty() const;
  bool is_valid() const;

  void clear();
  void fill(T value);
  void resize(Length width, Length height);
  void resize(Length width, Length height, Stride stride_bytes);
  void set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes, bool take_ownership = false);

  T* data();
  const T* data() const;

  T* data(Index y);
  const T* data(Index y) const;

  T* data_row_end(Index y);
  const T* data_row_end(Index y) const;

  T* data(Index x, Index y);
  const T* data(Index x, Index y) const;

  std::uint8_t* byte_ptr();
  const std::uint8_t* byte_ptr() const;

  std::uint8_t* byte_ptr(Index y);
  const std::uint8_t* byte_ptr(Index y) const;

  std::uint8_t* byte_ptr(Index x, Index y);
  const std::uint8_t* byte_ptr(Index x, Index y) const;

  T& operator()(Index x, Index y);
  const T& operator()(Index x, Index y) const;

private:
  std::uint8_t* data_;
  Stride stride_bytes_;
  Length width_;
  Length height_;
  bool owns_memory_;

  void allocate_bytes(std::size_t nr_bytes);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();
  void copy_rows_from(const Image<T>& src);
  std::uint32_t compute_data_offset(Index y) const;
  std::uint32_t compute_data_offset(Index x, Index y) const;

  std::uint8_t* relinquish_data_ownership();

  friend void clone<>(const Image<T>& src, Image<T>& dst);
  template <typename PixelType> friend ImageData to_image_data(Image<T>&&, PixelFormat);
};

template <typename T>
void clone(const Image<T>& src, Image<T>& dst);

template <typename T>
void clone(const Image<T>& src, Index x0, Index y0, Length width, Length height, Image<T>& dst);

template <typename T>
Image<T> clone(const Image<T>& src);

template <typename T>
Image<T> clone(const Image<T>& src, Index x0, Index y0, Length width, Length height);

template <typename T>
Image<T> view(const Image<T>& src);

template <typename T>
Image<T> view(const Image<T>& src, Index x0, Index y0, Length width, Length height);

template <typename T>
void crop(Image<T>& img, Index x0, Index y0, Length width, Length height);

// ----------
// Aliases:

using Image_8u1 = Image<Pixel<std::uint8_t, 1>>;
using Image_8u2 = Image<Pixel<std::uint8_t, 2>>;
using Image_8u3 = Image<Pixel<std::uint8_t, 3>>;
using Image_8u4 = Image<Pixel<std::uint8_t, 4>>;

using Image_8s1 = Image<Pixel<std::int8_t, 1>>;
using Image_8s2 = Image<Pixel<std::int8_t, 2>>;
using Image_8s3 = Image<Pixel<std::int8_t, 3>>;
using Image_8s4 = Image<Pixel<std::int8_t, 4>>;

using Image_16u1 = Image<Pixel<std::uint16_t, 1>>;
using Image_16u2 = Image<Pixel<std::uint16_t, 2>>;
using Image_16u3 = Image<Pixel<std::uint16_t, 3>>;
using Image_16u4 = Image<Pixel<std::uint16_t, 4>>;

using Image_16s1 = Image<Pixel<std::int16_t, 1>>;
using Image_16s2 = Image<Pixel<std::int16_t, 2>>;
using Image_16s3 = Image<Pixel<std::int16_t, 3>>;
using Image_16s4 = Image<Pixel<std::int16_t, 4>>;

using Image_32u1 = Image<Pixel<std::uint32_t, 1>>;
using Image_32u2 = Image<Pixel<std::uint32_t, 2>>;
using Image_32u3 = Image<Pixel<std::uint32_t, 3>>;
using Image_32u4 = Image<Pixel<std::uint32_t, 4>>;

using Image_32s1 = Image<Pixel<std::int32_t, 1>>;
using Image_32s2 = Image<Pixel<std::int32_t, 2>>;
using Image_32s3 = Image<Pixel<std::int32_t, 3>>;
using Image_32s4 = Image<Pixel<std::int32_t, 4>>;

using Image_64u1 = Image<Pixel<std::uint64_t, 1>>;
using Image_64u2 = Image<Pixel<std::uint64_t, 2>>;
using Image_64u3 = Image<Pixel<std::uint64_t, 3>>;
using Image_64u4 = Image<Pixel<std::uint64_t, 4>>;

using Image_64s1 = Image<Pixel<std::int64_t, 1>>;
using Image_64s2 = Image<Pixel<std::int64_t, 2>>;
using Image_64s3 = Image<Pixel<std::int64_t, 3>>;
using Image_64s4 = Image<Pixel<std::int64_t, 4>>;

using Image_32f1 = Image<Pixel<float32_t, 1>>;
using Image_32f2 = Image<Pixel<float32_t, 2>>;
using Image_32f3 = Image<Pixel<float32_t, 3>>;
using Image_32f4 = Image<Pixel<float32_t, 4>>;

using Image_64f1 = Image<Pixel<float64_t, 1>>;
using Image_64f2 = Image<Pixel<float64_t, 2>>;
using Image_64f3 = Image<Pixel<float64_t, 3>>;
using Image_64f4 = Image<Pixel<float64_t, 4>>;

// ----------
// Implementation:

template <typename T>
Image<T>::Image()
    : data_(nullptr), stride_bytes_(0), width_(0), height_(0), owns_memory_(true)
{
}

template <typename T>
Image<T>::Image(Length width, Length height)
    : data_(nullptr), stride_bytes_(PixelTraits<T>::nr_bytes * width),
      width_(width), height_(height), owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
}

template <typename T>
Image<T>::Image(Length width, Length height, T value)
    : data_(nullptr), stride_bytes_(PixelTraits<T>::nr_bytes * width),
      width_(width), height_(height), owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
  fill(value);
}

template <typename T>
Image<T>::Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes, bool take_ownership)
    : data_(data), stride_bytes_(stride_bytes), width_(width), height_(height), owns_memory_(take_ownership)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

template <typename T>
inline Image<T>::Image(const Image<T>& other)
    : data_(other.data_), stride_bytes_(other.stride_bytes_), width_(other.width_), height_(other.height_),
      owns_memory_(other.owns_memory_)
{
  // Keep the image semantics, i.e. owned will be owned, and non-owned will stay non-owned
  if (!other.owns_memory_ || other.is_empty())
  {
    return;
  }

  // Allocate memory to copy data from other image
  allocate_bytes(height_ * stride_bytes_);
  // Copy the image data
  copy_rows_from(other);

}

template <typename T>
inline Image<T>& Image<T>::operator=(const Image<T>& other)
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  // Deallocate own memory if we need to
  if (!equal_size || !other.owns_memory_)
  {
    deallocate_bytes_if_owned();
  }

  // Copy over values from other image
  data_ = other.data_;
  stride_bytes_ = other.stride_bytes_;
  width_ = other.width_;
  height_ = other.height_;
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    // Allocate if we deallocated
    if (!equal_size)
    {
      allocate_bytes(stride_bytes_ * height_);
    }

    // Copy the image data
    copy_rows_from(other);
  }

  return *this;
}

template <typename T>
inline Image<T>::Image(Image<T>&& other) noexcept
    : data_(other.data_), stride_bytes_(other.stride_bytes_), width_(other.width_), height_(other.height_),
      owns_memory_(other.owns_memory_)
{
  other.reset();
}

template <typename T>
inline Image<T>& Image<T>::operator=(Image<T>&& other) noexcept
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = other.data_;
  stride_bytes_ = other.stride_bytes_;
  width_ = other.width_;
  height_ = other.height_;
  owns_memory_ = other.owns_memory_;

  // Reset other image
  other.reset();

  return *this;
}

template <typename T>
inline Image<T>::~Image()
{
  deallocate_bytes_if_owned();
}

template <typename T>
inline Length Image<T>::width() const
{
  return width_;
}

template <typename T>
inline Length Image<T>::height() const
{
  return height_;
}

template <typename T>
inline Stride Image<T>::stride_bytes() const
{
  return stride_bytes_;
}

template <typename T>
inline std::size_t Image<T>::total_bytes() const
{
  return stride_bytes_ * height_;
}

template <typename T>
inline bool Image<T>::is_packed() const
{
  return stride_bytes_ == PixelTraits<T>::nr_bytes * width_;
}

template <typename T>
inline bool Image<T>::is_view() const
{
  return !owns_memory_;
}

template <typename T>
inline bool Image<T>::is_empty() const
{
  return data_ == nullptr || width_ == 0 || height_ == 0;
}

template <typename T>
inline bool Image<T>::is_valid() const
{
  return !is_empty();
}

template <typename T>
void Image<T>::clear()
{
  deallocate_bytes_if_owned();
  reset();
}

template <typename T>
void Image<T>::fill(T value)
{
  for (std::uint32_t y = 0; y < height_; ++y)
  {
    std::fill(data(y), data_row_end(y), value);
  }
}

template <typename T>
void Image<T>::resize(Length width, Length height)
{
  const auto stride_bytes = PixelTraits<T>::nr_bytes * width;
  resize(width, height, stride_bytes);
}

template <typename T>
void Image<T>::resize(Length width, Length height, Stride stride_bytes)
{
  // No need to act, if size parameters match
  if (width_ == width && height_ == height && stride_bytes_ == stride_bytes)
  {
    return;
  }

  if (!owns_memory_)
  {
    throw std::runtime_error("Cannot resize external data");
  }

  deallocate_bytes();
  allocate_bytes(stride_bytes * height);

  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

template <typename T>
inline void Image<T>::set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes, bool take_ownership)
{
  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data;
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = take_ownership;
}

template <typename T>
inline T* Image<T>::data()
{
  return reinterpret_cast<T*>(byte_ptr());
}

template <typename T>
inline const T* Image<T>::data() const
{
  return reinterpret_cast<const T*>(byte_ptr());
}

template <typename T>
inline T* Image<T>::data(Index y)
{
  return reinterpret_cast<T*>(byte_ptr(y));
}

template <typename T>
inline const T* Image<T>::data(Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(y));
}

template <typename T>
inline T* Image<T>::data_row_end(Index y)
{
  return reinterpret_cast<T*>(byte_ptr(y) + PixelTraits<T>::nr_bytes * width_);
}

template <typename T>
inline const T* Image<T>::data_row_end(Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(y) + PixelTraits<T>::nr_bytes * width_);
}

template <typename T>
inline T* Image<T>::data(Index x, Index y)
{
  return reinterpret_cast<T*>(byte_ptr(x, y));
}

template <typename T>
inline const T* Image<T>::data(Index x, Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(x, y));
}

template <typename T>
inline std::uint8_t* Image<T>::byte_ptr()
{
  return data_;
}

template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr() const
{
  return data_;
}

template <typename T>
inline std::uint8_t* Image<T>::byte_ptr(Index y)
{
  return data_ + compute_data_offset(y);
}

template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr(Index y) const
{
  return data_ + compute_data_offset(y);
}

template <typename T>
inline std::uint8_t* Image<T>::byte_ptr(Index x, Index y)
{
  return data_ + compute_data_offset(x, y);
}

template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr(Index x, Index y) const
{
  return data_ + compute_data_offset(x, y);
}

template <typename T>
inline T& Image<T>::operator()(Index x, Index y)
{
  return *data(x, y);
}

template <typename T>
inline const T& Image<T>::operator()(Index x, Index y) const
{
  return *data(x, y);
}

template <typename T>
void Image<T>::allocate_bytes(std::size_t nr_bytes)
{
  SELENE_ASSERT(owns_memory_);

  auto memory = DefaultAllocator::allocate(nr_bytes);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

template <typename T>
void Image<T>::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);

  if (data_)
  {
    DefaultAllocator::deallocate(data_);
    SELENE_ASSERT(data_ == nullptr);
  }
}

template <typename T>
void Image<T>::deallocate_bytes_if_owned()
{
  if (owns_memory_)
  {
    deallocate_bytes();
  }
}

template <typename T>
void Image<T>::reset()
{
  // reset to default constructed state
  data_ = nullptr;
  stride_bytes_ = Stride{0};
  width_ = Length{0};
  height_ = Length{0};
  owns_memory_ = true;
}

template <typename T>
void Image<T>::copy_rows_from(const Image<T>& src)
{
  SELENE_ASSERT(data_ && src.data_);
  SELENE_ASSERT(width_ == src.width_ && height_ == src.height_);

  for (Index y = 0; y < height_; ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename T>
inline std::uint32_t Image<T>::compute_data_offset(Index y) const
{
  return stride_bytes_ * y;
}

template <typename T>
inline std::uint32_t Image<T>::compute_data_offset(Index x, Index y) const
{
  return stride_bytes_ * y + PixelTraits<T>::nr_bytes * x;
}

template <typename T>
inline std::uint8_t* Image<T>::relinquish_data_ownership()
{
  auto ptr = data_;
  owns_memory_ = false;
  clear();
  return ptr;
}

// ----------

template <typename T>
void clone(const Image<T>& src, Image<T>& dst)
{
  if (&src == &dst)
  {
    throw std::runtime_error("Destination cannot be the same as the source for image cloning");
  }

  dst.resize(src.width(), src.height(), src.stride_bytes());
  dst.copy_rows_from(src);
}

template <typename T>
void clone(const Image<T>& src, Index x0, Index y0, Length width, Length height, Image<T>& dst)
{
  auto src_sub_view = view(src, x0, y0, width, height);
  clone(src_sub_view, dst);
}

template <typename T>
Image<T> clone(const Image<T>& src)
{
  Image<T> dst;
  clone(src, dst);
  return dst;
}

template <typename T>
Image<T> clone(const Image<T>& src, Index x0, Index y0, Length width, Length height)
{
  Image<T> dst;
  clone(src, x0, y0, width, height, dst);
  return dst;
}

template <typename T>
Image<T> view(const Image<T>& src)
{
  return Image<T>(src.byte_ptr(), src.width(), src.height(), src.stride_bytes());
}

template <typename T>
Image<T> view(const Image<T>& src, Index x0, Index y0, Length width, Length height)
{
  return Image<T>(src.byte_ptr(x0, y0), width, height, src.stride_bytes());
}

template <typename T>
void crop(Image<T>& img, Index x0, Index y0, Length width, Length height)
{
  auto cropped_clone = clone(img, x0, y0, width, height);
  img = std::move(cropped_clone);
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IMAGE_HPP
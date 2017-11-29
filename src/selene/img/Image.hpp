// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_HPP
#define SELENE_IMG_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

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

/** \brief Statically typed image class.
 *
 * An instance of `Image<T>` represents a statically typed image with pixel elements of type `T`.
 * Since the number of channels is determined by the pixel type `T` (e.g. `Pixel<T>`), the storage of multiple
 * channels/samples is always interleaved, as opposed to planar.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * The memory of an `Image<T>` instance may either be owned or non-owned; in the latter case, the instance is a "view"
 * on image data.
 */
template <typename T>
class Image
{
public:
  Image();
  Image(Length width, Length height);
  Image(Length width, Length height, Stride stride_bytes);
  Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes);
  Image(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes);

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
  void set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes);
  void set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes);

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

  MemoryBlock<NewAllocator> relinquish_data_ownership();

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

using Image_8u1 = Image<Pixel<std::uint8_t, 1>>;  ///< 8-bit unsigned 1-channel image.
using Image_8u2 = Image<Pixel<std::uint8_t, 2>>;  ///< 8-bit unsigned 2-channel image.
using Image_8u3 = Image<Pixel<std::uint8_t, 3>>;  ///< 8-bit unsigned 3-channel image.
using Image_8u4 = Image<Pixel<std::uint8_t, 4>>;  ///< 8-bit unsigned 4-channel image.

using Image_8s1 = Image<Pixel<std::int8_t, 1>>;  ///< 8-bit signed 1-channel image.
using Image_8s2 = Image<Pixel<std::int8_t, 2>>;  ///< 8-bit signed 2-channel image.
using Image_8s3 = Image<Pixel<std::int8_t, 3>>;  ///< 8-bit signed 3-channel image.
using Image_8s4 = Image<Pixel<std::int8_t, 4>>;  ///< 8-bit signed 4-channel image.

using Image_16u1 = Image<Pixel<std::uint16_t, 1>>;  ///< 16-bit unsigned 1-channel image.
using Image_16u2 = Image<Pixel<std::uint16_t, 2>>;  ///< 16-bit unsigned 2-channel image.
using Image_16u3 = Image<Pixel<std::uint16_t, 3>>;  ///< 16-bit unsigned 3-channel image.
using Image_16u4 = Image<Pixel<std::uint16_t, 4>>;  ///< 16-bit unsigned 4-channel image.

using Image_16s1 = Image<Pixel<std::int16_t, 1>>;  ///< 16-bit signed 1-channel image.
using Image_16s2 = Image<Pixel<std::int16_t, 2>>;  ///< 16-bit signed 2-channel image.
using Image_16s3 = Image<Pixel<std::int16_t, 3>>;  ///< 16-bit signed 3-channel image.
using Image_16s4 = Image<Pixel<std::int16_t, 4>>;  ///< 16-bit signed 4-channel image.

using Image_32u1 = Image<Pixel<std::uint32_t, 1>>;  ///< 32-bit unsigned 1-channel image.
using Image_32u2 = Image<Pixel<std::uint32_t, 2>>;  ///< 32-bit unsigned 2-channel image.
using Image_32u3 = Image<Pixel<std::uint32_t, 3>>;  ///< 32-bit unsigned 3-channel image.
using Image_32u4 = Image<Pixel<std::uint32_t, 4>>;  ///< 32-bit unsigned 4-channel image.

using Image_32s1 = Image<Pixel<std::int32_t, 1>>;  ///< 32-bit signed 1-channel image.
using Image_32s2 = Image<Pixel<std::int32_t, 2>>;  ///< 32-bit signed 2-channel image.
using Image_32s3 = Image<Pixel<std::int32_t, 3>>;  ///< 32-bit signed 3-channel image.
using Image_32s4 = Image<Pixel<std::int32_t, 4>>;  ///< 32-bit signed 4-channel image.

using Image_64u1 = Image<Pixel<std::uint64_t, 1>>;  ///< 64-bit unsigned 1-channel image.
using Image_64u2 = Image<Pixel<std::uint64_t, 2>>;  ///< 64-bit unsigned 2-channel image.
using Image_64u3 = Image<Pixel<std::uint64_t, 3>>;  ///< 64-bit unsigned 3-channel image.
using Image_64u4 = Image<Pixel<std::uint64_t, 4>>;  ///< 64-bit unsigned 4-channel image.

using Image_64s1 = Image<Pixel<std::int64_t, 1>>;  ///< 64-bit signed 1-channel image.
using Image_64s2 = Image<Pixel<std::int64_t, 2>>;  ///< 64-bit signed 2-channel image.
using Image_64s3 = Image<Pixel<std::int64_t, 3>>;  ///< 64-bit signed 3-channel image.
using Image_64s4 = Image<Pixel<std::int64_t, 4>>;  ///< 64-bit signed 4-channel image.

using Image_32f1 = Image<Pixel<float32_t, 1>>;  ///< 32-bit floating point 1-channel image.
using Image_32f2 = Image<Pixel<float32_t, 2>>;  ///< 32-bit floating point 2-channel image.
using Image_32f3 = Image<Pixel<float32_t, 3>>;  ///< 32-bit floating point 3-channel image.
using Image_32f4 = Image<Pixel<float32_t, 4>>;  ///< 32-bit floating point 4-channel image.

using Image_64f1 = Image<Pixel<float64_t, 1>>;  ///< 64-bit floating point 1-channel image.
using Image_64f2 = Image<Pixel<float64_t, 2>>;  ///< 64-bit floating point 2-channel image.
using Image_64f3 = Image<Pixel<float64_t, 3>>;  ///< 64-bit floating point 3-channel image.
using Image_64f4 = Image<Pixel<float64_t, 4>>;  ///< 64-bit floating point 4-channel image.

// ----------
// Implementation:

/** \brief Default constructor.
 *
 * Creates an empty image of width and height 0. The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam T The pixel type.
 */
template <typename T>
Image<T>::Image()
    : data_(nullptr), stride_bytes_(0), width_(0), height_(0), owns_memory_(true)
{
}

/** \brief Constructs an image of the specified width and height.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam T The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 */
template <typename T>
Image<T>::Image(Length width, Length height)
    : data_(nullptr), stride_bytes_(PixelTraits<T>::nr_bytes * width),
      width_(width), height_(height), owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
}

/** \brief Constructs an image of the specified width, height, and stride in bytes.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam T The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename T>
Image<T>::Image(Length width, Length height, Stride stride_bytes)
    : data_(nullptr), stride_bytes_(std::max(stride_bytes, PixelTraits<T>::nr_bytes * width)),
      width_(width), height_(height), owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
}

/** \brief Constructs an image view (non-owned data) from supplied memory.
 *
 * @tparam T The pixel type.
 * @param data Pointer to the existing image data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename T>
Image<T>::Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes)
    : data_(data), stride_bytes_(stride_bytes), width_(width), height_(height), owns_memory_(false)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

/** \brief Constructs an image (owned data) from supplied memory.
 *
 * @tparam T The pixel type.
 * @param data A `MemoryBlock<NewAllocator>` with the existing data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename T>
Image<T>::Image(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes)
    : data_(data.transfer_data()), stride_bytes_(stride_bytes), width_(width), height_(height),
      owns_memory_(true)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

/** \brief Copy constructor.
 *
 * Constructs an image instance from the supplied image.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @tparam T The pixel type.
 * @param other The source image.
 */
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

/** \brief Copy assignment operator
 *
 * Assigns another image instance.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @tparam T The pixel type.
 * @param other The image to assign from.
 * @return A reference to this image.
 */
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

/** \brief Move constructor.
 *
 * @tparam T The pixel type.
 * @param other The image to move from.
 */
template <typename T>
inline Image<T>::Image(Image<T>&& other) noexcept
    : data_(other.data_), stride_bytes_(other.stride_bytes_), width_(other.width_), height_(other.height_),
      owns_memory_(other.owns_memory_)
{
  other.reset();
}

/**\brief Move assignment operator.
 *
 * @tparam T The pixel type.
 * @param other The image to move assign from.
 * @return A reference to this image.
 */
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

/** \brief Destructor.
 *
 * Owned data will be deallocated at destruction time.
 *
 * @tparam T The pixel type.
 */
template <typename T>
inline Image<T>::~Image()
{
  deallocate_bytes_if_owned();
}

/** \brief Returns the image width.
 *
 * @tparam T The pixel type.
 * @return Width of the image in pixels.
 */
template <typename T>
inline Length Image<T>::width() const
{
  return width_;
}

/** \brief Returns the image height.
 *
 * @tparam T The pixel type.
 * @return Height of the image in pixels.
 */
template <typename T>
inline Length Image<T>::height() const
{
  return height_;
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * PixelTraits<T>::nr_bytes)`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @tparam T The pixel type.
 * @return Row stride in bytes.
 */
template <typename T>
inline Stride Image<T>::stride_bytes() const
{
  return stride_bytes_;
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam The pixel type.
 * @return Number of bytes occupied by the image data in memory.
 */
template <typename T>
inline std::size_t Image<T>::total_bytes() const
{
  return stride_bytes_ * height_;
}

/** \brief Returns whether the image data is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * PixelTraits<T>::nr_bytes)`.
 *
 * @tparam T The pixel type.
 * @return True, if the image data stored packed; false otherwise.
 */
template <typename T>
inline bool Image<T>::is_packed() const
{
  return stride_bytes_ == PixelTraits<T>::nr_bytes * width_;
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @tparam T The pixel type.
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
template <typename T>
inline bool Image<T>::is_view() const
{
  return !owns_memory_;
}

/** \brief Returns whether the image is empty.
 *
 * An image is considered empty if its internal data pointer points to `nullptr`, `width() == 0`, `height() == 0`, or
 * any combination of these.
 *
 * @tparam T The pixel type.
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename T>
inline bool Image<T>::is_empty() const
{
  return data_ == nullptr || width_ == 0 || height_ == 0;
}

/** \brief Returns whether the instance represents a valid image.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam T The pixel type.
 * @return True, if the image is valid; false otherwise.
 */
template <typename T>
inline bool Image<T>::is_valid() const
{
  return !is_empty();
}

/** \brief Resets the image instance by clearing the image data and resetting the internal state to the state after
 * default construction.
 *
 * Postconditions: `data() == nullptr && width() == 0 && height() == 0 && stride_bytes() == 0 && is_empty() &&
 * !is_valid() && !is_view()`.
 *
 * @tparam T The pixel type.
 */
template <typename T>
void Image<T>::clear()
{
  deallocate_bytes_if_owned();
  reset();
}

/** \brief Fills the image data, i.e. each pixel, with the specified value.
 *
 * @tparam T The pixel type.
 * @param value The value that each image pixel should assume.
 */
template <typename T>
void Image<T>::fill(T value)
{
  for (std::uint32_t y = 0; y < height_; ++y)
  {
    std::fill(data(y), data_row_end(y), value);
  }
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height).
 *
 * Precondition: `!is_view()`.
 *
 * Postconditions: `!is_view() && is_packed()`.
 *
 * Equivalent to `resize(width, height, width * PixelTraits<T>::nr_bytes`.
 * Images that are views onto non-owned memory cannot be resized. In this case, a `std::runtime_error` exception will
 * be thrown.
 *
 * @tparam T The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 */
template <typename T>
void Image<T>::resize(Length width, Length height)
{
  const auto stride_bytes = PixelTraits<T>::nr_bytes * width;
  resize(width, height, stride_bytes);
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height), with user-defined row
 * stride.
 *
 * Precondition: `!is_view()`.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= width() * PixelTraits<T>::nr_bytes)`.
 *
 * Images that are views onto non-owned memory cannot be resized. In this case, a `std::runtime_error` exception will
 * be thrown.
 *
 * No memory reallocation will happen, if the specified `width`, `height`, and `stride_bytes` parameters already match
 * the internal state.
 *
 * @tparam T The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 * @param stride_bytes The desired row stride in bytes.
 */
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

  stride_bytes = std::max(stride_bytes, PixelTraits<T>::nr_bytes * width);

  deallocate_bytes();
  allocate_bytes(stride_bytes * height);

  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

/** \brief Sets the image data to be a view onto non-owned external memory.
 *
 * Postcondition: `is_view()`.
 *
 * @tparam T The pixel type.
 * @param data Pointer to the external data.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename T>
inline void Image<T>::set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes)
{
  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data;
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = false;
}

/** \brief Sets the image data to the provided memory block, which will be owned by the `Image<>` instance.
 *
 * Precondition: `data.size() == stride_bytes * height`.
 *
 * Postcondition: `!is_view()`.
 *
 * @tparam T The pixel type.
 * @param data Memory block of image data, to be owned by the image instance.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename T>
inline void Image<T>::set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes)
{
  SELENE_ASSERT(data.size() == stride_bytes * height);

  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data.transfer_data();
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam T The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename T>
inline T* Image<T>::data()
{
  return reinterpret_cast<T*>(byte_ptr());
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam T The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename T>
inline const T* Image<T>::data() const
{
  return reinterpret_cast<const T*>(byte_ptr());
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename T>
inline T* Image<T>::data(Index y)
{
  return reinterpret_cast<T*>(byte_ptr(y));
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename T>
inline const T* Image<T>::data(Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(y));
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename T>
inline T* Image<T>::data_row_end(Index y)
{
  return reinterpret_cast<T*>(byte_ptr(y) + PixelTraits<T>::nr_bytes * width_);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y,
 * column `width()`).
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename T>
inline const T* Image<T>::data_row_end(Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(y) + PixelTraits<T>::nr_bytes * width_);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename T>
inline T* Image<T>::data(Index x, Index y)
{
  return reinterpret_cast<T*>(byte_ptr(x, y));
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename T>
inline const T* Image<T>::data(Index x, Index y) const
{
  return reinterpret_cast<const T*>(byte_ptr(x, y));
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam T The pixel type.
 * @return Pointer to the first image data byte.
 */
template <typename T>
inline std::uint8_t* Image<T>::byte_ptr()
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @tparam T The pixel type.
 * @return Constant pointer to the first image data byte.
 */
template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr() const
{
  return data_;
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename T>
inline std::uint8_t* Image<T>::byte_ptr(Index y)
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @tparam T The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr(Index y) const
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename T>
inline std::uint8_t* Image<T>::byte_ptr(Index x, Index y)
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column
 * `x`.
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename T>
inline const std::uint8_t* Image<T>::byte_ptr(Index x, Index y) const
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename T>
inline T& Image<T>::operator()(Index x, Index y)
{
  return *data(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam T The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename T>
inline const T& Image<T>::operator()(Index x, Index y) const
{
  return *data(x, y);
}

template <typename T>
void Image<T>::allocate_bytes(std::size_t nr_bytes)
{
  SELENE_ASSERT(owns_memory_);

  auto memory = NewAllocator::allocate(nr_bytes);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

template <typename T>
void Image<T>::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);

  if (data_)
  {
    NewAllocator::deallocate(data_);
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
inline MemoryBlock<NewAllocator> Image<T>::relinquish_data_ownership()
{
  SELENE_FORCED_ASSERT(owns_memory_);
  const auto ptr = data_;
  const auto len = total_bytes();

  owns_memory_ = false;
  clear();
  return construct_memory_block_from_existing_memory<NewAllocator>(ptr, len);
}

// ----------

/** \brief Copies the image represented by `src` into the image `dst`.
 *
 * After copying, `dst` will be owning its image data memory.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @param dst Destination image.
 */
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

/** \brief Copies the specified part of the source image `src` into the image `dst`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the region to be copied.
 * @param y0 The top-left y-coordinate of the region to be copied.
 * @param width The width of the region to be copied.
 * @param height The height of the region to be copied.
 * @param dst Destination image.
 */
template <typename T>
void clone(const Image<T>& src, Index x0, Index y0, Length width, Length height, Image<T>& dst)
{
  auto src_sub_view = view(src, x0, y0, width, height);
  clone(src_sub_view, dst);
}

/** \brief Returns a copy of the provided source image.
 *
 * After copying, the returned image will be owning its image data memory.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @return Copied image.
 */
template <typename T>
Image<T> clone(const Image<T>& src)
{
  Image<T> dst;
  clone(src, dst);
  return dst;
}

/** \brief Returns a copy of the specified sub-region of the source image `src`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region to be copied.
 * @param y0 The top-left y-coordinate of the sub-region to be copied.
 * @param width The width of the sub-region to be copied.
 * @param height The height of the sub-region to be copied.
 * @return Copied image, containing the sub-region.
 */
template <typename T>
Image<T> clone(const Image<T>& src, Index x0, Index y0, Length width, Length height)
{
  Image<T> dst;
  clone(src, x0, y0, width, height, dst);
  return dst;
}

/** \brief Returns an image representing a view onto the provided source image.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image.
 */
template <typename T>
Image<T> view(const Image<T>& src)
{
  return Image<T>(src.byte_ptr(), src.width(), src.height(), src.stride_bytes());
}

/** \brief Returns an image representing a view onto the specified sub-region of the provided source image.
 *
 * @tparam T The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image sub-region.
 */
template <typename T>
Image<T> view(const Image<T>& src, Index x0, Index y0, Length width, Length height)
{
  return Image<T>(src.byte_ptr(x0, y0), width, height, src.stride_bytes());
}

/** \brief Crops the image `img` to the specified sub-region.
 *
 * There is no explicit check whether the crop region is within the image bounds. Specifying an invalid region results
 * in undefined behavior.
 *
 * @tparam T The pixel type.
 * @param img The image to be cropped.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 */
template <typename T>
void crop(Image<T>& img, Index x0, Index y0, Length width, Length height)
{
  auto cropped_clone = clone(img, x0, y0, width, height);
  img = std::move(cropped_clone);
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IMAGE_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_HPP
#define SELENE_IMG_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/old_img/ImageDataStorage.hpp>
#include <selene/old_img/ImageIterators.hpp>
#include <selene/old_img/PixelFormat.hpp>
#include <selene/old_img/PixelTraits.hpp>
#include <selene/old_img/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

namespace sln {

template <ImageDataStorage storage_type>
class ImageData;

template <typename PixelType>
void clone(const Image<PixelType>&, Image<PixelType>&);

template <typename PixelTypeSrc, typename PixelTypeDst, typename = std::enable_if_t<!std::is_same<PixelTypeSrc, PixelTypeDst>::value>>
void clone(const Image<PixelTypeSrc>&, Image<PixelTypeDst>&);

template <typename PixelType>
ImageData<ImageDataStorage::Modifiable> to_image_data(Image<PixelType>&&, PixelFormat = PixelFormat::Invalid);


/** \brief Statically typed image class.
 *
 * An instance of `Image<PixelType>` represents a statically typed image with pixel elements of type `PixelType`.
 * Since the number of channels is determined by the pixel type (e.g. `Pixel<U, N>`), the storage of multiple
 * channels/samples is always interleaved, as opposed to planar.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * The memory of an `Image<PixelType>` instance may either be owned or non-owned; in the latter case, the instance is a
 * "view" on image data.
 */
template <typename PixelType_>
class Image
{
public:
  using PixelType = PixelType_;  ///< The pixel type.
  using DataPtrType = std::conditional_t<std::is_const<PixelType>::value, const std::uint8_t*, std::uint8_t*>;
  using iterator = ImageRowIterator<PixelType>;  ///< The iterator type.
  using const_iterator = ConstImageRowIterator<PixelType>;  ///< The const_iterator type.

  Image();
  Image(PixelLength width, PixelLength height, Stride stride_bytes = Stride{0});
  Image(PixelLength width, PixelLength height, ImageRowAlignment row_alignment_bytes);
  Image(DataPtrType data, PixelLength width, PixelLength height, Stride stride_bytes = Stride{0}) noexcept;
  Image(MemoryBlock<AlignedNewAllocator>&& data,
        PixelLength width,
        PixelLength height,
        Stride stride_bytes = Stride{0}) noexcept;

  Image(const Image<PixelType>& other);
  Image<PixelType>& operator=(const Image<PixelType>& other);

  Image(Image<PixelType>&& other) noexcept;
  Image<PixelType>& operator=(Image<PixelType>&& other) noexcept;

  ~Image();

  PixelLength width() const noexcept;
  PixelLength height() const noexcept;
  Stride stride_bytes() const noexcept;
  std::size_t row_bytes() const noexcept;
  std::size_t total_bytes() const noexcept;
  bool is_packed() const noexcept;
  bool is_view() const noexcept;
  bool is_empty() const noexcept;
  bool is_valid() const noexcept;

  void clear() noexcept;
  void fill(PixelType value) noexcept;
  void allocate(PixelLength width,
                PixelLength height,
                Stride stride_bytes = Stride{0},
                bool shrink_to_fit = true,
                bool force_allocation = false,
                bool allow_view_reallocation = true);
  void allocate(PixelLength width,
                PixelLength height,
                ImageRowAlignment row_alignment_bytes,
                bool shrink_to_fit = true,
                bool force_allocation = false,
                bool allow_view_reallocation = true);
  void maybe_allocate(PixelLength width, PixelLength height, Stride stride_bytes = Stride{0});
  void set_view(std::uint8_t* data, PixelLength width, PixelLength height, Stride stride_bytes = Stride{0});
  void set_data(MemoryBlock<AlignedNewAllocator>&& data,
                PixelLength width,
                PixelLength height,
                Stride stride_bytes = Stride{0});

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  DataPtrType byte_ptr() noexcept;
  const DataPtrType byte_ptr() const noexcept;

  DataPtrType byte_ptr(PixelIndex y) noexcept;
  const DataPtrType byte_ptr(PixelIndex y) const noexcept;

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept;
  const DataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

  PixelType* data() noexcept;
  const PixelType* data() const noexcept;

  PixelType* data(PixelIndex y) noexcept;
  const PixelType* data(PixelIndex y) const noexcept;

  PixelType* data_row_end(PixelIndex y) noexcept;
  const PixelType* data_row_end(PixelIndex y) const noexcept;

  PixelType* data(PixelIndex x, PixelIndex y) noexcept;
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept;

  PixelType& operator()(PixelIndex x, PixelIndex y) noexcept;
  const PixelType& operator()(PixelIndex x, PixelIndex y) const noexcept;

private:
  static_assert(std::is_trivial<PixelType>::value, "Pixel type is not trivial");
  static_assert(std::is_standard_layout<PixelType>::value, "Pixel type is not standard layout");

  std::uint8_t* data_ = nullptr;
  Stride stride_bytes_;
  PixelLength width_;
  PixelLength height_;
  bool owns_memory_ = true;

  constexpr static std::size_t default_base_alignment_ = 16;

  void allocate(PixelLength width,
                PixelLength height,
                Stride stride_bytes,
                std::size_t base_alignment_bytes,
                bool shrink_to_fit,
                bool force_allocation,
                bool allow_view_reallocation);
  void allocate_bytes(std::size_t nr_bytes, std::size_t alignment);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();
  template <typename PixelTypeSrc> void copy_rows_from(const Image<PixelTypeSrc>& src);
  Bytes compute_data_offset(PixelIndex y) const noexcept;
  Bytes compute_data_offset(PixelIndex x, PixelIndex y) const noexcept;

  MemoryBlock<AlignedNewAllocator> relinquish_data_ownership();

  template <typename PixelType>
    friend void clone(const Image<PixelType>&, Image<PixelType>&);

  template <typename PixelTypeSrc, typename PixelTypeDst, typename>
    friend void clone(const Image<PixelTypeSrc>&, Image<PixelTypeDst>&);

  friend ImageData<ImageDataStorage::Modifiable> to_image_data<PixelType>(Image<PixelType>&&, PixelFormat);
  friend class ImageRowIterator<Image<PixelType>>;
  friend class ConstImageRowIterator<Image<PixelType>>;
};

template <typename PixelType>
bool operator==(const Image<PixelType>& img0, const Image<PixelType>& img1);

template <typename PixelType>
bool operator!=(const Image<PixelType>& img0, const Image<PixelType>& img1);

//template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
//void clone(const Image<PixelTypeSrc>& src, Image<PixelTypeDst>& dst);

template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
void clone(const Image<PixelTypeSrc>& src,
           PixelIndex x0,
           PixelIndex y0,
           PixelLength width,
           PixelLength height,
           Image<PixelTypeDst>& dst);

template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
Image<PixelTypeDst> clone(const Image<PixelTypeSrc>& src);

template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
Image<PixelTypeDst> clone(const Image<PixelTypeSrc>& src,
                          PixelIndex x0,
                          PixelIndex y0,
                          PixelLength width,
                          PixelLength height);

template <typename PixelTypeSrc, typename PixelTypeDst = ConstifyPixel_t<PixelTypeSrc>>
Image<PixelTypeDst> view(const Image<PixelTypeSrc>&);

template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
Image<PixelTypeDst> view(Image<PixelTypeSrc>&);

template <typename PixelTypeSrc, typename PixelTypeDst = ConstifyPixel_t<PixelTypeSrc>>
Image<PixelTypeDst> view(const Image<PixelTypeSrc>&, PixelIndex, PixelIndex, PixelLength, PixelLength);

template <typename PixelTypeSrc, typename PixelTypeDst = PixelTypeSrc>
Image<PixelTypeDst> view(Image<PixelTypeSrc>&, PixelIndex, PixelIndex, PixelLength, PixelLength);

template <typename PixelType>
void crop(Image<PixelType>& img, PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height);

// ----------
// Implementation:

/** \brief Default constructor.
 *
 * Creates an empty image of width and height 0. The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
Image<PixelType>::Image() : stride_bytes_(0), width_(0), height_(0)
{
}

/** \brief Constructs an image of the specified width, height, and stride in bytes.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * The image (row) data is not guaranteed to be aligned in any way.
 *
 * @tparam PixelType The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(PixelLength width, PixelLength height, Stride stride_bytes)
    : stride_bytes_(std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width)))
    , width_(width)
    , height_(height)
{
  constexpr auto base_alignment_bytes = Image<PixelType>::default_base_alignment_;
  allocate_bytes(stride_bytes_ * height_, base_alignment_bytes);
}

/** \brief Constructs an image of the specified width, height, and with the specified row alignment.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * The row stride (in bytes) is chosen to be the smallest value that satisfies the row alignment requirements.
 *
 * @tparam PixelType The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param row_alignment_bytes The row alignment in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(PixelLength width, PixelLength height, ImageRowAlignment row_alignment_bytes)
    : stride_bytes_(impl::compute_stride_bytes(PixelTraits<PixelType>::nr_bytes * width, row_alignment_bytes))
    , width_(width)
    , height_(height)
{
  allocate_bytes(stride_bytes_ * height_, row_alignment_bytes);
}

/** \brief Constructs an image view (non-owned data) from supplied memory.
 *
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * @tparam PixelType The pixel type.
 * @param data Pointer to the existing image data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(DataPtrType data, PixelLength width, PixelLength height, Stride stride_bytes) noexcept
    : data_(const_cast<std::uint8_t*>(data))  // HACK  // NOLINT
    , stride_bytes_(std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width)))
    , width_(width)
    , height_(height)
    , owns_memory_(false)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

/** \brief Constructs an image (owned data) from supplied memory.
 *
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * @tparam PixelType The pixel type.
 * @param data A `MemoryBlock<NewAllocator>` with the existing data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(MemoryBlock<AlignedNewAllocator>&& data,
                        PixelLength width,
                        PixelLength height,
                        Stride stride_bytes) noexcept
    : data_(data.transfer_data())
    , stride_bytes_(std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width)))
    , width_(width)
    , height_(height)
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
 * @tparam PixelType The pixel type.
 * @param other The source image.
 */
template <typename PixelType>
inline Image<PixelType>::Image(const Image<PixelType>& other)
    : data_(other.data_)
    , stride_bytes_(other.stride_bytes_)
    , width_(other.width_)
    , height_(other.height_)
    , owns_memory_(other.owns_memory_)
{
  // Keep the image semantics, i.e. owned will be owned, and non-owned will stay non-owned
  if (!other.owns_memory_ || other.is_empty())
  {
    return;
  }

  // Allocate memory to copy data from other image
  allocate_bytes(height_ * stride_bytes_, impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                                      other.stride_bytes()));
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
 * @tparam PixelType The pixel type.
 * @param other The image to assign from.
 * @return A reference to this image.
 */
template <typename PixelType>
inline Image<PixelType>& Image<PixelType>::operator=(const Image<PixelType>& other)
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
      allocate_bytes(stride_bytes_ * height_,
                     impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                 other.stride_bytes()));
    }

    // Copy the image data
    copy_rows_from(other);
  }

  return *this;
}

/** \brief Move constructor.
 *
 * @tparam PixelType The pixel type.
 * @param other The image to move from.
 */
template <typename PixelType>
inline Image<PixelType>::Image(Image<PixelType>&& other) noexcept
    : data_(other.data_)
    , stride_bytes_(other.stride_bytes_)
    , width_(other.width_)
    , height_(other.height_)
    , owns_memory_(other.owns_memory_)
{
  other.reset();
}

/**\brief Move assignment operator.
 *
 * @tparam PixelType The pixel type.
 * @param other The image to move assign from.
 * @return A reference to this image.
 */
template <typename PixelType>
inline Image<PixelType>& Image<PixelType>::operator=(Image<PixelType>&& other) noexcept
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
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
inline Image<PixelType>::~Image()
{
  deallocate_bytes_if_owned();
}

/** \brief Returns the image width.
 *
 * @tparam PixelType The pixel type.
 * @return Width of the image in pixels.
 */
template <typename PixelType>
inline PixelLength Image<PixelType>::width() const noexcept
{
  return width_;
}

/** \brief Returns the image height.
 *
 * @tparam PixelType The pixel type.
 * @return Height of the image in pixels.
 */
template <typename PixelType>
inline PixelLength Image<PixelType>::height() const noexcept
{
  return height_;
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @tparam PixelType The pixel type.
 * @return Row stride in bytes.
 */
template <typename PixelType>
inline Stride Image<PixelType>::stride_bytes() const noexcept
{
  return stride_bytes_;
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * PixelTraits::nr_bytes)`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @tparam PixelType The pixel type.
 * @return Number of data bytes occupied by each image row.
 */
template <typename PixelType>
inline std::size_t Image<PixelType>::row_bytes() const noexcept
{
  return width_ * PixelTraits<PixelType>::nr_bytes;
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam PixelType The pixel type.
 * @return Number of bytes occupied by the image data in memory.
 */
template <typename PixelType>
inline std::size_t Image<PixelType>::total_bytes() const noexcept
{
  return stride_bytes_ * height_;
}

/** \brief Returns whether the image data is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image data stored packed; false otherwise.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_packed() const noexcept
{
  return stride_bytes_ == static_cast<Stride::value_type>(PixelTraits<PixelType>::nr_bytes * width_);
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_view() const noexcept
{
  return !owns_memory_;
}

/** \brief Returns whether the image is empty.
 *
 * An image is considered empty if its internal data pointer points to `nullptr`, `width() == 0`, `height() == 0`, or
 * any combination of these.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_empty() const noexcept
{
  return data_ == nullptr || width_ == 0 || height_ == 0;
}

/** \brief Returns whether the instance represents a valid image.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image is valid; false otherwise.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_valid() const noexcept
{
  return !is_empty();
}

/** \brief Resets the image instance by clearing the image data and resetting the internal state to the state after
 * default construction.
 *
 * Postconditions: `data() == nullptr && width() == 0 && height() == 0 && stride_bytes() == 0 && is_empty() &&
 * !is_valid() && !is_view()`.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
void Image<PixelType>::clear() noexcept
{
  deallocate_bytes_if_owned();
  reset();
}

/** \brief Fills the image data, i.e. each pixel, with the specified value.
 *
 * @tparam PixelType The pixel type.
 * @param value The value that each image pixel should assume.
 */
template <typename PixelType>
void Image<PixelType>::fill(PixelType value) noexcept
{
  for (PixelIndex y = 0_idx; y < height_; ++y)
  {
    std::fill(data(y), data_row_end(y), value);
  }
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height), with user-defined row
 * stride.
 *
 * No memory (re)allocation will happen, if the needed allocation size already matches the existing allocation size.
 * See also the `shrink_to_fit` parameter.
 *
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 * @param stride_bytes The desired row stride in bytes.
 * @param shrink_to_fit If true, reallocate if it results in less memory usage; otherwise allow excess memory to stay
 * allocated
 * @param force_allocation If true, always force a reallocation. Overrides `allow_view_reallocation == false`.
 * @param allow_view_reallocation If true, allow allocation from `is_view() == true`. If false, and the existing image
 * is a view, a `std::runtime_error` exception will be thrown (respecting the strong exception guarantee).
 */
template <typename PixelType>
void Image<PixelType>::allocate(PixelLength width,
                                PixelLength height,
                                Stride stride_bytes,
                                bool shrink_to_fit,
                                bool force_allocation,
                                bool allow_view_reallocation)
{
  constexpr auto base_alignment_bytes = Image<PixelType>::default_base_alignment_;
  allocate(width, height, stride_bytes, base_alignment_bytes, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height), with user-defined row
 * alignment.
 *
 * No memory (re)allocation will happen, if the needed allocation size already matches the existing allocation size.
 * See also the `shrink_to_fit` parameter.
 *
 * The row stride (in bytes) is chosen to be the smallest stride that satisfied the row alignment.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 * @param row_alignment_bytes The desired row alignment in bytes.
 * @param shrink_to_fit If true, reallocate if it results in less memory usage; otherwise allow excess memory to stay
 * allocated
 * @param force_allocation If true, always force a reallocation. Overrides `allow_view_reallocation == false`.
 * @param allow_view_reallocation If true, allow allocation from `is_view() == true`. If false, and the existing image
 * is a view, a `std::runtime_error` exception will be thrown (respecting the strong exception guarantee).
 */
template <typename PixelType>
void Image<PixelType>::allocate(PixelLength width,
                                PixelLength height,
                                ImageRowAlignment row_alignment_bytes,
                                bool shrink_to_fit,
                                bool force_allocation,
                                bool allow_view_reallocation)
{
  const auto row_bytes = width * PixelTraits<PixelType>::nr_bytes;
  const auto stride_bytes = impl::compute_stride_bytes(row_bytes, row_alignment_bytes);
  allocate(width, height, stride_bytes, row_alignment_bytes, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height), with user-defined row
 * stride, if (and only if) the existing width and height differ (disregarding the existing stride).
 *
 * No memory (re)allocation will happen, if the specified `width` and `height` parameters already match the internal
 * state.
 *
 * If an allocation takes place, the row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`,
 * or the supplied value.
 *
 * If the existing image is a view (`is_view()`), and the pointed-to memory region would need to be changed in size to
 * conform to the desired `width` and `height` parameters, a `std::runtime_error` exception will be thrown (respecting
 * the strong exception guarantee).
 *
 * Postconditions: `!is_view() && (stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType The pixel type.
 * @param width The desired image width.
 * @param height The desired image height.
 * @param stride_bytes The desired row stride in bytes, if (and only if) an allocation takes place.
 */
template <typename PixelType>
void Image<PixelType>::maybe_allocate(PixelLength width, PixelLength height, Stride stride_bytes)
{
  if (width_ == width && height_ == height)
  {
    return;
  }

  constexpr auto base_alignment_bytes = Image<PixelType>::default_base_alignment_;
  constexpr auto shrink_to_fit = true;
  constexpr auto force_allocation = false;
  constexpr auto allow_view_reallocation = false;
  allocate(width, height, stride_bytes, base_alignment_bytes, shrink_to_fit, force_allocation, allow_view_reallocation);
}

/** \brief Sets the image data to be a view onto non-owned external memory.
 *
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * Postcondition: `is_view()`.
 *
 * @tparam PixelType The pixel type.
 * @param data Pointer to the external data.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename PixelType>
inline void Image<PixelType>::set_view(std::uint8_t* data, PixelLength width, PixelLength height, Stride stride_bytes)
{
  stride_bytes = std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width));

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
 * The row stride (in bytes) is chosen to be at least `width * PixelTraits::nr_bytes`, or the supplied value.
 *
 * Precondition: `data.size() >= stride_bytes * height`.
 *
 * Postcondition: `!is_view()`.
 *
 * @tparam PixelType The pixel type.
 * @param data Memory block of image data, to be owned by the image instance.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename PixelType>
inline void Image<PixelType>::set_data(MemoryBlock<AlignedNewAllocator>&& data,
                                       PixelLength width,
                                       PixelLength height,
                                       Stride stride_bytes)
{
  stride_bytes = std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width));
  SELENE_ASSERT(data.size() >= stride_bytes * height);

  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data.transfer_data();
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

/** \brief Returns an iterator to the first row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Iterator to the first image row.
 */
template <typename PixelType>
inline typename Image<PixelType>::iterator Image<PixelType>::begin() noexcept
{
  return ImageRowIterator<PixelType>(ImageRow<PixelType>(this, 0_idx));
}

/** \brief Returns a const iterator to the first row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Const iterator to the first image row.
 */
template <typename PixelType>
inline typename Image<PixelType>::const_iterator Image<PixelType>::begin() const noexcept
{
  return ConstImageRowIterator<PixelType>(ConstImageRow<PixelType>(this, 0_idx));
}

/** \brief Returns a const iterator to the first row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Const iterator to the first image row.
 */
template <typename PixelType>
inline typename Image<PixelType>::const_iterator Image<PixelType>::cbegin() const noexcept
{
  return ConstImageRowIterator<PixelType>(ConstImageRow<PixelType>(this, 0_idx));
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Iterator to image row after the last row.
 */
template <typename PixelType>
inline typename Image<PixelType>::iterator Image<PixelType>::end() noexcept
{
  return ImageRowIterator<PixelType>(ImageRow<PixelType>(this, PixelIndex{height_}));
}

/** \brief Returns a const iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Const iterator to image row after the last row.
 */
template <typename PixelType>
inline typename Image<PixelType>::const_iterator Image<PixelType>::end() const noexcept
{
  return ConstImageRowIterator<PixelType>(ConstImageRow<PixelType>(this, PixelIndex{height_}));
}

/** \brief Returns a const iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Const iterator to image row after the last row.
 */
template <typename PixelType>
inline typename Image<PixelType>::const_iterator Image<PixelType>::cend() const noexcept
{
  return ConstImageRowIterator<PixelType>(ConstImageRow<PixelType>(this, PixelIndex{height_}));
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first image data byte.
 */
template <typename PixelType>
inline typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr() noexcept
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first image data byte.
 */
template <typename PixelType>
inline const typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr() const noexcept
{
  return data_;
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename PixelType>
inline typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr(PixelIndex y) noexcept
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename PixelType>
inline const typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr(PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr(PixelIndex x, PixelIndex y) noexcept
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column
 * `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline const typename Image<PixelType>::DataPtrType Image<PixelType>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data() noexcept
{
  return reinterpret_cast<PixelType*>(byte_ptr());
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data() const noexcept
{
  return reinterpret_cast<const PixelType*>(byte_ptr());
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data(PixelIndex y) noexcept
{
  return reinterpret_cast<PixelType*>(byte_ptr(y));
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data(PixelIndex y) const noexcept
{
  return reinterpret_cast<const PixelType*>(byte_ptr(y));
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data_row_end(PixelIndex y) noexcept
{
  return reinterpret_cast<PixelType*>(byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * width_);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y,
 * column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data_row_end(PixelIndex y) const noexcept
{
  return reinterpret_cast<const PixelType*>(byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * width_);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data(PixelIndex x, PixelIndex y) noexcept
{
  return reinterpret_cast<PixelType*>(byte_ptr(x, y));
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data(PixelIndex x, PixelIndex y) const noexcept
{
  return reinterpret_cast<const PixelType*>(byte_ptr(x, y));
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline PixelType& Image<PixelType>::operator()(PixelIndex x, PixelIndex y) noexcept
{
  return *data(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline const PixelType& Image<PixelType>::operator()(PixelIndex x, PixelIndex y) const noexcept
{
  return *data(x, y);
}

template <typename PixelType>
void Image<PixelType>::allocate(PixelLength width,
                                PixelLength height,
                                Stride stride_bytes,
                                std::size_t base_alignment_bytes,
                                bool shrink_to_fit,
                                bool force_allocation,
                                bool allow_view_reallocation)
{
  stride_bytes = std::max(stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * width));
  const auto nr_bytes_to_allocate = stride_bytes * height;
  const auto nr_currently_allocated_bytes = total_bytes();

  auto commit_new_geometry = [=]() {
    width_ = width;
    height_ = height;
    stride_bytes_ = stride_bytes;
  };

  // No need to act, if size parameters match
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

template <typename PixelType>
void Image<PixelType>::allocate_bytes(std::size_t nr_bytes, std::size_t alignment)
{
  SELENE_ASSERT(owns_memory_);

  auto memory = AlignedNewAllocator::allocate(nr_bytes, alignment);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

template <typename PixelType>
void Image<PixelType>::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);

  if (data_)
  {
    AlignedNewAllocator::deallocate(data_);
    SELENE_ASSERT(data_ == nullptr);
  }
}

template <typename PixelType>
void Image<PixelType>::deallocate_bytes_if_owned()
{
  if (owns_memory_)
  {
    deallocate_bytes();
  }
}

template <typename PixelType>
void Image<PixelType>::reset()
{
  // reset to default constructed state
  data_ = nullptr;
  stride_bytes_ = Stride{0};
  width_ = PixelLength{0};
  height_ = PixelLength{0};
  owns_memory_ = true;
}

template <typename PixelType>
template <typename PixelTypeSrc>
void Image<PixelType>::copy_rows_from(const Image<PixelTypeSrc>& src)
{
  SELENE_ASSERT(data_ && src.data());
  SELENE_ASSERT(width_ == src.width() && height_ == src.height());

  for (PixelIndex y = 0_idx; y < height_; ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType>
inline Bytes Image<PixelType>::compute_data_offset(PixelIndex y) const noexcept
{
  return Bytes(stride_bytes_ * y);
}

template <typename PixelType>
inline Bytes Image<PixelType>::compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
{
  return Bytes(stride_bytes_ * y + PixelTraits<PixelType>::nr_bytes * x);
}

template <typename PixelType>
inline MemoryBlock<AlignedNewAllocator> Image<PixelType>::relinquish_data_ownership()
{
  SELENE_FORCED_ASSERT(owns_memory_);
  const auto ptr = data_;
  const auto len = total_bytes();

  owns_memory_ = false;
  clear();
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, len);
}

// ----------

/** \brief Equality comparison for two images.
 *
 * @tparam PixelType The pixel type.
 * @param img0 The left-hand side image to compare.
 * @param img1 The right-hand side image to compare.
 * @return True, if the two images have equal extents and equal pixel values in all rows and columns; false otherwise.
 */
template <typename PixelType>
bool operator==(const Image<PixelType>& img0, const Image<PixelType>& img1)
{
  // Special case: if both images have a zero-length side, the shall be considered equal (both are invalid)
  if ((img0.width() == 0 || img0.height() == 0) && (img1.width() == 0 || img1.height() == 0))
  {
    return true;
  }

  if (img0.width() != img1.width() || img0.height() != img1.height())
  {
    return false;
  }

  for (auto y = 0_idx; y < img0.height(); ++y)
  {
    const auto end0 = img0.data_row_end(y);
    const auto begin0 = img0.data(y);
    const auto begin1 = img1.data(y);

    // std::equal may not be optimized to std::memcmp, even though we're dealing with a POD-type here...
    // const bool equal_row = std::equal(begin0, end0, begin1);
    // ...so let's just call std::memcmp directly:
    const auto nr_bytes = std::distance(begin0, end0) * sizeof(PixelType);
    const bool equal_row = (std::memcmp(begin0, begin1, nr_bytes) == 0);

    if (!equal_row)
    {
      return false;
    }
  }

  return true;
}

/** \brief Inequality comparison for two images.
 *
 * @tparam PixelType The pixel type.
 * @param img0 The left-hand side image to compare.
 * @param img1 The right-hand side image to compare.
 * @return True, if the two images are not equal; false otherwise.
 */
template <typename PixelType>
bool operator!=(const Image<PixelType>& img0, const Image<PixelType>& img1)
{
  return !(img0 == img1);
}

/** \brief Copies the image represented by `src` into the image `dst`.
 *
 * After copying, `dst` will be owning its image data memory.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param dst Destination image.
 */
template <typename PixelType>
void clone(const Image<PixelType>& src, Image<PixelType>& dst)
{
  if (&src == &dst)
  {
    throw std::runtime_error("Destination cannot be the same as the source for image cloning");
  }

  if (!src.is_valid())
  {
    dst.clear();
    return;
  }

  dst.allocate(src.width(), src.height(), src.stride_bytes());
  dst.copy_rows_from(src);
}

template <typename PixelTypeSrc, typename PixelTypeDst, typename>
void clone(const Image<PixelTypeSrc>& src, Image<PixelTypeDst>& dst)
{
  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<std::remove_cv_t<typename PixelTraits<PixelTypeSrc>::Element>,
                             std::remove_cv_t<typename PixelTraits<PixelTypeDst>::Element>>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                  == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                 || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                 || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  //if (&src == &dst)
  //{
  //  throw std::runtime_error("Destination cannot be the same as the source for image cloning");
  //}

  if (!src.is_valid())
  {
    dst.clear();
    return;
  }

  dst.allocate(src.width(), src.height(), src.stride_bytes());
  dst.copy_rows_from(src);
}

/** \brief Copies the specified part of the source image `src` into the image `dst`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the region to be copied.
 * @param y0 The top-left y-coordinate of the region to be copied.
 * @param width The width of the region to be copied.
 * @param height The height of the region to be copied.
 * @param dst Destination image.
 */
template <typename PixelTypeSrc, typename PixelTypeDst>
void clone(const Image<PixelTypeSrc>& src,
           PixelIndex x0,
           PixelIndex y0,
           PixelLength width,
           PixelLength height,
           Image<PixelTypeDst>& dst)
{
  auto src_sub_view = view(src, x0, y0, width, height);
  clone(src_sub_view, dst);
}

/** \brief Returns a copy of the provided source image.
 *
 * After copying, the returned image will be owning its image data memory.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @return Copied image.
 */
template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> clone(const Image<PixelTypeSrc>& src)
{
  Image<PixelTypeDst> dst;
  clone(src, dst);
  return dst;
}

/** \brief Returns a copy of the specified sub-region of the source image `src`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region to be copied.
 * @param y0 The top-left y-coordinate of the sub-region to be copied.
 * @param width The width of the sub-region to be copied.
 * @param height The height of the sub-region to be copied.
 * @return Copied image, containing the sub-region.
 */
template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> clone(const Image<PixelTypeSrc>& src, PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height)
{
  Image<PixelTypeDst> dst;
  clone(src, x0, y0, width, height, dst);
  return dst;
}

template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> view(const Image<PixelTypeSrc>& src)
{
  using ConstPixelTypeSrc = ConstifyPixel_t<PixelTypeSrc>;

  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<ConstPixelTypeSrc>::Element,
                             typename PixelTraits<PixelTypeDst>::Element>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                  == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                 || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                 || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  return Image<PixelTypeDst>(src.byte_ptr(), src.width(), src.height(), src.stride_bytes());
}

/** \brief Returns an image representing a view onto the provided source image.
 *
 * The target pixel format can be optionally changed (e.g. from/to PixelFormat::Unknown), if compatible source and
 * target pixel types are specified.
 *
 * @tparam PixelTypeSrc The source pixel type. Will be automatically deduced, if target pixel type is not given.
 * @tparam PixelTypeDst The target pixel type.
 * @param src Source image.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image.
 */
template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> view(Image<PixelTypeSrc>& src)
{
  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<PixelTypeSrc>::Element,
                             typename PixelTraits<PixelTypeDst>::Element>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                  == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                 || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                 || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  return Image<PixelTypeDst>(src.byte_ptr(), src.width(), src.height(), src.stride_bytes());
}

template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> view(const Image<PixelTypeSrc>& src, PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height)
{
  using ConstPixelTypeSrc = ConstifyPixel_t<PixelTypeSrc>;

  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<ConstPixelTypeSrc>::Element,
                             typename PixelTraits<PixelTypeDst>::Element>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                  == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                 || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                 || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  const auto data_offset = Bytes(src.stride_bytes() * y0 + PixelTraits<PixelTypeSrc>::nr_bytes * x0);
  return Image<PixelTypeDst>(src.byte_ptr() + data_offset, width, height, src.stride_bytes());
}

/** \brief Returns an image representing a view onto the specified sub-region of the provided source image.
 *
 * The target pixel format can be optionally changed (e.g. from/to PixelFormat::Unknown), if compatible source and
 * target pixel types are specified.
 *
 * @tparam PixelTypeSrc The source pixel type. Will be automatically deduced, if target pixel type is not given.
 * @tparam PixelTypeDst The target pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image sub-region.
 */
template <typename PixelTypeSrc, typename PixelTypeDst>
Image<PixelTypeDst> view(Image<PixelTypeSrc>& src, PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height)
{
  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<PixelTypeSrc>::Element,
                             typename PixelTraits<PixelTypeDst>::Element>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                  == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                 || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                 || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  const auto data_offset = Bytes(src.stride_bytes() * y0 + PixelTraits<PixelTypeSrc>::nr_bytes * x0);
  return Image<PixelTypeDst>(src.byte_ptr() + data_offset, width, height, src.stride_bytes());
}

/** \brief Crops the image `img` to the specified sub-region.
 *
 * There is no explicit check whether the crop region is within the image bounds. Specifying an invalid region results
 * in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to be cropped.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 */
template <typename PixelType>
void crop(Image<PixelType>& img, PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height)
{
  auto cropped_clone = clone(img, x0, y0, width, height);
  img = std::move(cropped_clone);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_HPP

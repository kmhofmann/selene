// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_HPP
#define SELENE_IMG_TYPED_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/typed/ImageView.hpp>

namespace sln {

template <typename PixelType_>
class Image;

template <typename PixelType0, typename PixelType1>
bool operator==(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1);

template <typename PixelType0, typename PixelType1>
bool operator!=(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1);

template <typename PixelType0, typename PixelType1>
bool equal(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1);

template <typename PixelType0, typename PixelType1, ImageModifiability modifiability>
bool equal(const Image<PixelType0>& img_0, const ImageView<PixelType1, modifiability>& img_view_1);

template <typename PixelType0, typename PixelType1, ImageModifiability modifiability>
bool equal(const ImageView<PixelType0, modifiability>& img_view_0, const Image<PixelType1>& img_1);

template <typename PixelType_>
constexpr void swap(Image<PixelType_>& img_l, Image<PixelType_>& img_r) noexcept;

template <typename PixelType_>
constexpr void swap(Image<PixelType_>& img_l, Image<PixelType_>& img_r) noexcept;

/** \brief Statically typed image class.
 *
 * An instance of `Image<PixelType>` represents a statically typed image with pixel elements of type `PixelType`.
 * Since the number of channels is determined by the pixel type (e.g. `Pixel<U, N>`), the storage of multiple
 * channels/samples is always interleaved, as opposed to planar.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * The memory of an `Image<PixelType>` instance is always owned by the instance.
 * To express a non-owning relation to the underlying data, use an `ImageView<PixelType, modifiability>`.
 *
 * @tparam PixelType_ The pixel type. Usually of type `Pixel<>`.
 */
template <typename PixelType_>
class Image
    : public ImageBase<Image<PixelType_>>
{
public:
  using PixelType = PixelType_;
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  using iterator = ImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The iterator type.
  using const_iterator = ConstImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The const_iterator type.

  constexpr static bool is_view = impl::ImageBaseTraits<Image<PixelType>>::is_view;
  constexpr static bool is_modifiable = impl::ImageBaseTraits<Image<PixelType>>::is_modifiable;

  constexpr static ImageModifiability modifiability()
  {
    return impl::ImageBaseTraits<Image<PixelType>>::modifiability();
  }

  Image() = default;  ///< Default constructor.

  explicit Image(TypedLayout layout);

  Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes);

  Image(MemoryBlock<AlignedNewAllocator>&& memory, TypedLayout layout);

  ~Image();

  Image(const Image<PixelType>&);

  Image<PixelType>& operator=(const Image<PixelType>&);

  Image(Image<PixelType>&&) noexcept;

  Image<PixelType>& operator=(Image<PixelType>&&) noexcept;

  template <ImageModifiability modifiability>
  explicit Image(const ImageView<PixelType, modifiability>&);

  template <ImageModifiability modifiability>
  Image<PixelType>& operator=(const ImageView<PixelType, modifiability>&);

  const TypedLayout& layout() const noexcept;

  PixelLength width() const noexcept;
  PixelLength height() const noexcept;
  Stride stride_bytes() const noexcept;
  std::ptrdiff_t row_bytes() const noexcept;
  std::ptrdiff_t total_bytes() const noexcept;

  bool is_packed() const noexcept;
  bool is_empty() const noexcept;
  bool is_valid() const noexcept;

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  DataPtrType byte_ptr() noexcept;
  ConstDataPtrType byte_ptr() const noexcept;

  DataPtrType byte_ptr(PixelIndex y) noexcept;
  ConstDataPtrType byte_ptr(PixelIndex y) const noexcept;

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept;
  ConstDataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

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

  ImageView<PixelType, ImageModifiability::Mutable>& view() noexcept;
  ImageView<PixelType, ImageModifiability::Constant> view() const noexcept;
  ImageView<PixelType, ImageModifiability::Constant> constant_view() const noexcept;

  void clear();

  bool reallocate(TypedLayout layout, ImageRowAlignment row_alignment_bytes, bool shrink_to_fit = true);

  MemoryBlock<AlignedNewAllocator> relinquish_data_ownership();

private:
  constexpr static auto default_base_alignment_bytes = ImageRowAlignment{16ul};

  ImageView<PixelType, ImageModifiability::Mutable> view_;

  template <typename Derived>
  void copy_rows_from(const ImageBase<Derived>& src);

  ImageView<PixelType, ImageModifiability::Mutable>
  allocate_memory(TypedLayout layout, std::ptrdiff_t base_alignment_bytes, std::ptrdiff_t row_alignment_bytes);

  void deallocate_memory();

  friend void swap<PixelType_>(Image<PixelType_>& img_l, Image<PixelType_>& img_r) noexcept;
};

// ----------
// Implementation:

/** \brief Constructs an image with the specified layout.
 *
 * @tparam PixelType_ The pixel type.
 * @param layout The image layout.
 */
template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, 0))
{
}

/** \brief Constructs an image with the specified layout and row alignment.
 *
 * @tparam PixelType_ The pixel type.
 * @param layout The image layout.
 * @param row_alignment_bytes The row alignment in bytes.
 */
template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes))
{
}

/** \brief Constructs an image with the specified layout, from an existing block of memory.
 *
 * On construction, the memory will be owned by the image instance.
 *
 * @tparam PixelType_ The pixel type.
 * @param memory The memory block representing the image.
 * @param layout The image layout.
 */
template <typename PixelType_>
Image<PixelType_>::Image(MemoryBlock<AlignedNewAllocator>&& memory, TypedLayout layout)
    : view_(memory.transfer_data(), layout)
{
}

/** \brief Destructor.
 *
 * All owned memory will be deallocated.
 *
 * @tparam PixelType_ The pixel type.
 */
template <typename PixelType_>
Image<PixelType_>::~Image()
{
  this->deallocate_memory();
}

/** \brief Copy constructor.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be copied from.
 */
template <typename PixelType_>
Image<PixelType_>::Image(const Image<PixelType>& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be assigned from.
 * @return A reference to this image.
 */
template <typename PixelType_>
Image<PixelType_>& Image<PixelType_>::operator=(const Image<PixelType>& other)
{
  // Check for self-assignment
  if (this == & other)
  {
    return * this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    view_ = allocate_memory(
        other.layout(),
        default_base_alignment_bytes,
        impl::guess_row_alignment(
            reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
            other.stride_bytes()));
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Move constructor.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be moved from.
 */
template <typename PixelType_>
Image<PixelType_>::Image(Image<PixelType>&& other) noexcept
    : view_(other.view_)
{
  other.view_ = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr},
                                                                  {PixelLength{0}, PixelLength{0}, Stride{0}}};
}

/** \brief Move assignment operator.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be move-assigned from.
 * @return A reference to this image.
 */
template <typename PixelType_>
Image<PixelType_>& Image<PixelType_>::operator=(Image<PixelType>&& other) noexcept
{
  // Check for self-assignment
  if (this == & other)
  {
    return * this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_ = other.view_;
  other.view_ = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr},
                                                                  {PixelLength{0}, PixelLength{0}, Stride{0}}};

  return * this;
}

/** \brief Copy constructor taking an `ImageView` of arbitrary modifiability.
 *
 * @tparam PixelType_ The pixel type.
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <typename PixelType_>
template <ImageModifiability modifiability_>
Image<PixelType_>::Image(const ImageView<PixelType, modifiability_>& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator taking an `ImageView` of arbitrary modifiability.
 *
 * @tparam PixelType_ The pixel type.
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be assigned from.
 */
template <typename PixelType_>
template <ImageModifiability modifiability_>
Image<PixelType_>& Image<PixelType_>::operator=(const ImageView<PixelType, modifiability_>& other)
{
  // Check for self-assignment
  if (& this->view_ == & other)
  {
    return * this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    view_ = allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()));
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Returns the image layout.
 *
 * @tparam PixelType_ The pixel type.
 * @return The typed image layout.
 */
template <typename PixelType_>
const TypedLayout& Image<PixelType_>::layout() const noexcept
{
  return view_.layout();
}

/** \brief Returns the image width.
 *
 * @tparam PixelType_ The pixel type.
 * @return The image width.
 */
template <typename PixelType_>
PixelLength Image<PixelType_>::width() const noexcept
{
  return view_.width();
}

/** \brief Returns the image height.
 *
 * @tparam PixelType_ The pixel type.
 * @return The image height.
 */
template <typename PixelType_>
PixelLength Image<PixelType_>::height() const noexcept
{
  return view_.height();
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @tparam PixelType_ The pixel type.
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The row stride of the image in bytes.
 */
template <typename PixelType_>
Stride Image<PixelType_>::stride_bytes() const noexcept
{
  return view_.stride_bytes();
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * PixelTraits::nr_bytes)`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @tparam PixelType_ The pixel type.
 * @return The number of data bytes occupied by each image row.
 */
template <typename PixelType_>
std::ptrdiff_t Image<PixelType_>::row_bytes() const noexcept
{
  return view_.row_bytes();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam PixelType_ The pixel type.
 * @return The total number of bytes occupied by the image data in memory.
 */
template <typename PixelType_>
std::ptrdiff_t Image<PixelType_>::total_bytes() const noexcept
{
  return view_.total_bytes();
}

/** \brief Returns whether the image is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image data is stored packed; false otherwise.
 */
template <typename PixelType_>
bool Image<PixelType_>::is_packed() const noexcept
{
  return view_.is_packed();
}

/** \brief Returns whether the image is empty.
 *
 * An image [view] is considered empty if its internal data pointer points to `nullptr`, `width() == 0`,
 * `height() == 0`, or any combination of these.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename PixelType_>
bool Image<PixelType_>::is_empty() const noexcept
{
  return view_.is_empty();
}

/** \brief Returns whether the image is valid.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image is valid; false otherwise.
 */
template <typename PixelType_>
bool Image<PixelType_>::is_valid() const noexcept
{
  return view_.is_valid();
}

/** \brief Returns an iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Iterator to the first image row.
 */
template <typename PixelType_>
auto Image<PixelType_>::begin() noexcept -> iterator
{
  return view_.begin();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType_>
auto Image<PixelType_>::begin() const noexcept -> const_iterator
{
  return view_.begin();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType_>
auto Image<PixelType_>::cbegin() const noexcept -> const_iterator
{
  return view_.cbegin();
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Iterator to the image row after the last row.
 */
template <typename PixelType_>
auto Image<PixelType_>::end() noexcept -> iterator
{
  return view_.end();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType_>
auto Image<PixelType_>::end() const noexcept -> const_iterator
{
  return view_.end();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType_>
auto Image<PixelType_>::cend() const noexcept -> const_iterator
{
  return view_.cend();
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Pointer to the first image data byte.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr() noexcept -> DataPtrType
{
  return view_.byte_ptr();
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant pointer to the first image data byte.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr() const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr();
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr(PixelIndex y) noexcept -> DataPtrType
{
  return view_.byte_ptr(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr(PixelIndex y) const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr(PixelIndex x, PixelIndex y) noexcept -> DataPtrType
{
  return view_.byte_ptr(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType_>
auto Image<PixelType_>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename PixelType_>
auto Image<PixelType_>::data() noexcept -> PixelType*
{
  return view_.data();
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename PixelType_>
auto Image<PixelType_>::data() const noexcept -> const PixelType*
{
  return view_.data();
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data(PixelIndex y) noexcept -> PixelType*
{
  return view_.data(y);
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data(PixelIndex y) const noexcept -> const PixelType*
{
  return view_.data(y);
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data_row_end(PixelIndex y) noexcept -> PixelType*
{
  return view_.data_row_end(y);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data_row_end(PixelIndex y) const noexcept -> const PixelType*
{
  return view_.data_row_end(y);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data(PixelIndex x, PixelIndex y) noexcept -> PixelType*
{
  return view_.data(x, y);
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType_>
auto Image<PixelType_>::data(PixelIndex x, PixelIndex y) const noexcept -> const PixelType*
{
  return view_.data(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType_>
auto Image<PixelType_>::operator()(PixelIndex x, PixelIndex y) noexcept -> PixelType&
{
  return view_.operator()(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType_>
auto Image<PixelType_>::operator()(PixelIndex x, PixelIndex y) const noexcept -> const PixelType&
{
  return view_.operator()(x, y);
}

/** \brief Returns the underlying (mutable) image view.
 *
 * @tparam PixelType_ The pixel type.
 * @return The underlying (mutable) image view.
 */
template <typename PixelType_>
auto Image<PixelType_>::view() noexcept
    -> ImageView<PixelType, ImageModifiability::Mutable>&
{
  return view_.view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @tparam PixelType_ The pixel type.
 * @return A constant image view.
 */
template <typename PixelType_>
auto Image<PixelType_>::view() const noexcept
    -> ImageView<PixelType, ImageModifiability::Constant>
{
  return view_.constant_view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @tparam PixelType_ The pixel type.
 * @return A constant image view.
 */
template <typename PixelType_>
auto Image<PixelType_>::constant_view() const noexcept
    -> ImageView<PixelType, ImageModifiability::Constant>
{
  return view_.constant_view();
}

/** \brief Clears the image; i.e. resets the internal state to the image state after default construction.
 *
 * All allocated memory will be deallocated.
 *
 * @tparam PixelType_ The pixel type.
 */
template <typename PixelType_>
void Image<PixelType_>::clear()
{
  deallocate_memory();
  view_.clear();
}

/** \brief Reallocates the image data according to the specified layout and alignment.
 *
 * @tparam PixelType_ The pixel type.
 * @param layout The layout for reallocation.
 * @param row_alignment_bytes The desired row alignment in bytes.
 * @param shrink_to_fit If true, then a reallocation will also take place if the required memory is less than already
 *                      allocated. If false, then the already allocated memory will be kept, if the required memory is
 *                      less than already allocated.
 * @return True, if a memory reallocation took place; false otherwise.
 */
template <typename PixelType_>
bool Image<PixelType_>::reallocate(TypedLayout layout, ImageRowAlignment row_alignment_bytes, bool shrink_to_fit)
{
  if (layout == this->view_.layout())
  {
    return false;
  }

  layout.stride_bytes = impl::compute_stride_bytes(
      std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width)),
      row_alignment_bytes);
  const auto nr_bytes_to_allocate = layout.stride_bytes * layout.height;
  const auto nr_currently_allocated_bytes = this->stride_bytes() * this->height();

  // No need to act if size parameters match
  const auto bytes_match = shrink_to_fit ? (nr_bytes_to_allocate == nr_currently_allocated_bytes)
                                         : (nr_bytes_to_allocate <= nr_currently_allocated_bytes);
  if (bytes_match)
  {
    view_ = ImageView<PixelType, ImageModifiability::Mutable>(this->byte_ptr(), layout);
    return false;
  }

  this->deallocate_memory();
  view_ = this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes);
  return true;
}

/** \brief Releases the owned memory from the image instance and returns it.
 *
 * As a result, the image will be empty, and no memory will be owned.
 *
 * @tparam PixelType_ The pixel type.
 * @return The owned memory block.
 */
template <typename PixelType_>
MemoryBlock<AlignedNewAllocator> Image<PixelType_>::relinquish_data_ownership()
{
  const auto ptr = this->byte_ptr();
  const auto len = this->total_bytes();

  view_.clear();
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, static_cast<std::size_t>(len));
}

template <typename PixelType_>
template <typename Derived>
void Image<PixelType_>::copy_rows_from(const ImageBase<Derived>& src)
{
  SELENE_ASSERT(data() && src.data());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < view_.height(); ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType_>
ImageView<PixelType_, ImageModifiability::Mutable> Image<PixelType_>::allocate_memory(
    TypedLayout layout,
    std::ptrdiff_t base_alignment_bytes,
    std::ptrdiff_t row_alignment_bytes)
{
  const auto stride_bytes = impl::compute_stride_bytes(
      std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width)), row_alignment_bytes);
  const auto nr_bytes_to_allocate = stride_bytes * layout.height;

  base_alignment_bytes = std::max(row_alignment_bytes, base_alignment_bytes);
  auto memory = sln::AlignedNewAllocator::allocate(
      static_cast<std::size_t>(nr_bytes_to_allocate),
      static_cast<std::size_t>(base_alignment_bytes));
  SELENE_ASSERT(static_cast<std::ptrdiff_t>(memory.size()) == nr_bytes_to_allocate);

  return ImageView<PixelType, ImageModifiability::Mutable>{{memory.transfer_data()},
                                                           {layout.width, layout.height, stride_bytes}};
}

template <typename PixelType_>
void Image<PixelType_>::deallocate_memory()
{
  std::uint8_t* ptr = view_.byte_ptr();
  sln::AlignedNewAllocator::deallocate(ptr);
}

// -----

template <typename PixelType0, typename PixelType1>
bool operator==(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1)
{
  return equal(img_0.view(), img_1.view());
}

template <typename PixelType0, typename PixelType1>
bool operator!=(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1)
{
  return !(img_0 == img_1);
}

template <typename PixelType0, typename PixelType1>
bool equal(const Image<PixelType0>& img_0, const Image<PixelType1>& img_1)
{
  return equal(img_0.view(), img_1.view());
}

template <typename PixelType0, typename PixelType1, ImageModifiability modifiability>
bool equal(const Image<PixelType0>& img_0, const ImageView<PixelType1, modifiability>& img_view_1)
{
  return equal(img_0.view(), img_view_1);
}

template <typename PixelType0, typename PixelType1, ImageModifiability modifiability>
bool equal(const ImageView<PixelType0, modifiability>& img_view_0, const Image<PixelType1>& img_1)
{
  return equal(img_view_0, img_1.view());
}

template <typename PixelType_>
constexpr void swap(Image<PixelType_>& img_l, Image<PixelType_>& img_r) noexcept
{
  using std::swap;
  swap(img_l.view_, img_r.view_);
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_HPP

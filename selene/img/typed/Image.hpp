// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_HPP
#define SELENE_IMG_TYPED_IMAGE_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>
#include <selene/base/_impl/CompressedPair.hpp>

#include <selene/img/typed/ImageView.hpp>

#include <memory>

namespace sln {

/// \addtogroup group-img-typed
/// @{

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool operator==(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1);

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool operator!=(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1);

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool equal(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1);

template <typename PixelType0, typename PixelType1, typename Allocator, ImageModifiability modifiability>
bool equal(const Image<PixelType0, Allocator>& img_0, const ImageView<PixelType1, modifiability>& img_view_1);

template <typename PixelType0, typename PixelType1, typename Allocator, ImageModifiability modifiability>
bool equal(const ImageView<PixelType0, modifiability>& img_view_0, const Image<PixelType1, Allocator>& img_1);

template <typename PixelType, typename Allocator>
void swap(Image<PixelType, Allocator>& img_l, Image<PixelType, Allocator>& img_r) noexcept;

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
template <typename PixelType_, typename Allocator_>
class Image
    : public ImageBase<Image<PixelType_, Allocator_>>
{
public:
  using PixelType = PixelType_;
  using Allocator = Allocator_;
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  using iterator = typename ImageView<PixelType_, ImageModifiability::Mutable>::iterator;  ///< The iterator type.
  using const_iterator = typename ImageView<PixelType_, ImageModifiability::Mutable>::const_iterator;  ///< The const_iterator type.

  constexpr static bool is_view = false;
  constexpr static bool is_modifiable = true;

  constexpr static ImageModifiability modifiability()
  {
    return ImageModifiability::Mutable;
  }

  Image() = default;  ///< Default constructor.

  explicit Image(const Allocator& alloc);

  explicit Image(TypedLayout layout);

  explicit Image(TypedLayout layout, const Allocator& alloc);

  Image(std::uint8_t* memory, TypedLayout layout, const Allocator& alloc = Allocator{});

  ~Image();

  Image(const Image<PixelType, Allocator>&);

  Image<PixelType, Allocator>& operator=(const Image<PixelType, Allocator>&);

  Image(Image<PixelType, Allocator>&&) noexcept;

  Image<PixelType, Allocator>& operator=(Image<PixelType, Allocator>&&) noexcept;

  template <ImageModifiability modifiability>
  explicit Image(const ImageView<PixelType, modifiability>&, const Allocator& alloc = Allocator{});

  template <ImageModifiability modifiability>
  Image<PixelType>& operator=(const ImageView<PixelType, modifiability>&);

  template <typename ImgExpr>
  explicit Image(const ImageExpr<ImgExpr>& expr);

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

  bool reallocate(TypedLayout layout);

  MemoryBlock<Allocator> relinquish_data_ownership();

private:
  impl::CompressedPair<ImageView<PixelType, ImageModifiability::Mutable>, Allocator> view_and_alloc_;

  ImageView<PixelType, ImageModifiability::Mutable>& mem_view() { return view_and_alloc_.first(); }
  const ImageView<PixelType, ImageModifiability::Mutable>& mem_view() const { return view_and_alloc_.first(); }

  Allocator& mem_alloc() { return view_and_alloc_.second(); }
  const Allocator& mem_alloc() const { return view_and_alloc_.second(); }

  template <typename Derived>
  void copy_rows_from(const ImageBase<Derived>& src);

  ImageView<PixelType, ImageModifiability::Mutable>
  allocate_memory(TypedLayout layout);

  void deallocate_memory();

  friend void swap<PixelType, Allocator>(Image<PixelType, Allocator>& img_l, Image<PixelType, Allocator>& img_r) noexcept;
};

/// @}

// ----------
// Implementation:

template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(const Allocator& alloc)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, alloc)
{
}

template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(TypedLayout layout)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, Allocator{})
{
  mem_view() = this->allocate_memory(layout);
}

/** \brief Constructs an image with the specified layout.
 *
 * @tparam PixelType_ The pixel type.
 * @param layout The image layout.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(TypedLayout layout, const Allocator& alloc)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, alloc)
{
  mem_view() = this->allocate_memory(layout);
}

/** \brief Constructs an image with the specified layout, from an existing block of memory.
 *
 * On construction, the memory will be owned by the image instance.
 *
 * @tparam PixelType_ The pixel type.
 * @param memory The memory block representing the image.
 * @param layout The image layout.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(std::uint8_t* memory, TypedLayout layout, const Allocator& alloc)
    : view_and_alloc_({memory, layout}, alloc)
{
}

/** \brief Destructor.
 *
 * All owned memory will be deallocated.
 *
 * @tparam PixelType_ The pixel type.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::~Image()
{
  this->deallocate_memory();
}

/** \brief Copy constructor.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be copied from.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(const Image<PixelType, Allocator>& other)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, other.mem_alloc())
{
  mem_view() = allocate_memory(other.layout());
  copy_rows_from(other);
}

/** \brief Copy assignment operator.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be assigned from.
 * @return A reference to this image.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>& Image<PixelType_, Allocator_>::operator=(const Image<PixelType, Allocator>& other)
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
    mem_alloc() = other.mem_alloc();
    mem_view() = allocate_memory(other.layout());
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Move constructor.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be moved from.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>::Image(Image<PixelType, Allocator>&& other) noexcept
    : view_and_alloc_(other.view_and_alloc_)
{
  other.mem_view() = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr},
                                                                       {PixelLength{0}, PixelLength{0}, Stride{0}}};
}

/** \brief Move assignment operator.
 *
 * @tparam PixelType_ The pixel type.
 * @param other The image to be move-assigned from.
 * @return A reference to this image.
 */
template <typename PixelType_, typename Allocator_>
Image<PixelType_, Allocator_>& Image<PixelType_, Allocator_>::operator=(Image<PixelType, Allocator>&& other) noexcept
{
  // Check for self-assignment
  if (this == & other)
  {
    return * this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_and_alloc_ = other.view_and_alloc_;
  other.mem_view() = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr},
                                                                       {PixelLength{0}, PixelLength{0}, Stride{0}}};
  return * this;
}

/** \brief Copy constructor taking an `ImageView` of arbitrary modifiability.
 *
 * @tparam PixelType_ The pixel type.
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <typename PixelType_, typename Allocator_>
template <ImageModifiability modifiability_>
Image<PixelType_, Allocator_>::Image(const ImageView<PixelType, modifiability_>& other, const Allocator& alloc)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, alloc)
{
  mem_view() = allocate_memory(other.layout());
  copy_rows_from(other);
}

/** \brief Copy assignment operator taking an `ImageView` of arbitrary modifiability.
 *
 * @tparam PixelType_ The pixel type.
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be assigned from.
 */
template <typename PixelType_, typename Allocator_>
template <ImageModifiability modifiability_>
Image<PixelType_>& Image<PixelType_, Allocator_>::operator=(const ImageView<PixelType, modifiability_>& other)
{
  // Check for self-assignment
  if (&this->mem_view() == &other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    mem_view() = allocate_memory(other.layout());
  }

  copy_rows_from(other);

  return *this;
}

template <typename PixelType_, typename Allocator_>
template <typename ImgExpr>
Image<PixelType_, Allocator_>::Image(const ImageExpr<ImgExpr>& expr)
    : view_and_alloc_(ImageView<PixelType, ImageModifiability::Mutable>{}, Allocator{})
{
  mem_view() = allocate_memory(expr.layout());

  // TODO: optimize?
  for (auto y = 0_idx; y < expr.height(); ++y)
  {
    for (auto x = 0_idx; x < expr.width(); ++x)
    {
      this->operator()(x, y) = expr(x, y);
    }
  }
}

/** \brief Returns the image layout.
 *
 * @tparam PixelType_ The pixel type.
 * @return The typed image layout.
 */
template <typename PixelType_, typename Allocator_>
const TypedLayout& Image<PixelType_, Allocator_>::layout() const noexcept
{
  return mem_view().layout();
}

/** \brief Returns the image width.
 *
 * @tparam PixelType_ The pixel type.
 * @return The image width.
 */
template <typename PixelType_, typename Allocator_>
PixelLength Image<PixelType_, Allocator_>::width() const noexcept
{
  return mem_view().width();
}

/** \brief Returns the image height.
 *
 * @tparam PixelType_ The pixel type.
 * @return The image height.
 */
template <typename PixelType_, typename Allocator_>
PixelLength Image<PixelType_, Allocator_>::height() const noexcept
{
  return mem_view().height();
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
template <typename PixelType_, typename Allocator_>
Stride Image<PixelType_, Allocator_>::stride_bytes() const noexcept
{
  return mem_view().stride_bytes();
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * PixelTraits::nr_bytes)`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @tparam PixelType_ The pixel type.
 * @return The number of data bytes occupied by each image row.
 */
template <typename PixelType_, typename Allocator_>
std::ptrdiff_t Image<PixelType_, Allocator_>::row_bytes() const noexcept
{
  return mem_view().row_bytes();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam PixelType_ The pixel type.
 * @return The total number of bytes occupied by the image data in memory.
 */
template <typename PixelType_, typename Allocator_>
std::ptrdiff_t Image<PixelType_, Allocator_>::total_bytes() const noexcept
{
  return mem_view().total_bytes();
}

/** \brief Returns whether the image is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image data is stored packed; false otherwise.
 */
template <typename PixelType_, typename Allocator_>
bool Image<PixelType_, Allocator_>::is_packed() const noexcept
{
  return mem_view().is_packed();
}

/** \brief Returns whether the image is empty.
 *
 * An image [view] is considered empty if its internal data pointer points to `nullptr`, `width() == 0`,
 * `height() == 0`, or any combination of these.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename PixelType_, typename Allocator_>
bool Image<PixelType_, Allocator_>::is_empty() const noexcept
{
  return mem_view().is_empty();
}

/** \brief Returns whether the image is valid.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam PixelType_ The pixel type.
 * @return True, if the image is valid; false otherwise.
 */
template <typename PixelType_, typename Allocator_>
bool Image<PixelType_, Allocator_>::is_valid() const noexcept
{
  return mem_view().is_valid();
}

/** \brief Returns an iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Iterator to the first image row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::begin() noexcept -> iterator
{
  return mem_view().begin();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::begin() const noexcept -> const_iterator
{
  return mem_view().begin();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::cbegin() const noexcept -> const_iterator
{
  return mem_view().cbegin();
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Iterator to the image row after the last row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::end() noexcept -> iterator
{
  return mem_view().end();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::end() const noexcept -> const_iterator
{
  return mem_view().end();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::cend() const noexcept -> const_iterator
{
  return mem_view().cend();
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Pointer to the first image data byte.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr() noexcept -> DataPtrType
{
  return mem_view().byte_ptr();
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant pointer to the first image data byte.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr() const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr();
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr(PixelIndex y) noexcept -> DataPtrType
{
  return mem_view().byte_ptr(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr(PixelIndex y) const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr(PixelIndex x, PixelIndex y) noexcept -> DataPtrType
{
  return mem_view().byte_ptr(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data() noexcept -> PixelType*
{
  return mem_view().data();
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data() const noexcept -> const PixelType*
{
  return mem_view().data();
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data(PixelIndex y) noexcept -> PixelType*
{
  return mem_view().data(y);
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data(PixelIndex y) const noexcept -> const PixelType*
{
  return mem_view().data(y);
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data_row_end(PixelIndex y) noexcept -> PixelType*
{
  return mem_view().data_row_end(y);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType_ The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data_row_end(PixelIndex y) const noexcept -> const PixelType*
{
  return mem_view().data_row_end(y);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data(PixelIndex x, PixelIndex y) noexcept -> PixelType*
{
  return mem_view().data(x, y);
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::data(PixelIndex x, PixelIndex y) const noexcept -> const PixelType*
{
  return mem_view().data(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::operator()(PixelIndex x, PixelIndex y) noexcept -> PixelType&
{
  return mem_view().operator()(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType_ The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::operator()(PixelIndex x, PixelIndex y) const noexcept -> const PixelType&
{
  return mem_view().operator()(x, y);
}

/** \brief Returns the underlying (mutable) image view.
 *
 * @tparam PixelType_ The pixel type.
 * @return The underlying (mutable) image view.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::view() noexcept
    -> ImageView<PixelType, ImageModifiability::Mutable>&
{
  return mem_view().view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @tparam PixelType_ The pixel type.
 * @return A constant image view.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::view() const noexcept
    -> ImageView<PixelType, ImageModifiability::Constant>
{
  return mem_view().constant_view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @tparam PixelType_ The pixel type.
 * @return A constant image view.
 */
template <typename PixelType_, typename Allocator_>
auto Image<PixelType_, Allocator_>::constant_view() const noexcept
    -> ImageView<PixelType, ImageModifiability::Constant>
{
  return mem_view().constant_view();
}

/** \brief Clears the image; i.e. resets the internal state to the image state after default construction.
 *
 * All allocated memory will be deallocated.
 *
 * @tparam PixelType_ The pixel type.
 */
template <typename PixelType_, typename Allocator_>
void Image<PixelType_, Allocator_>::clear()
{
  deallocate_memory();
  mem_view().clear();
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
template <typename PixelType_, typename Allocator_>
bool Image<PixelType_, Allocator_>::reallocate(TypedLayout layout)
{
  if (layout == this->mem_view().layout())
  {
    return false;
  }

  layout.stride_bytes = std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width));

  this->deallocate_memory();
  mem_view() = this->allocate_memory(layout);
  return true;
}

/** \brief Releases the owned memory from the image instance and returns it.
 *
 * As a result, the image will be empty, and no memory will be owned.
 *
 * @tparam PixelType_ The pixel type.
 * @return The owned memory block.
 */
template <typename PixelType_, typename Allocator_>
MemoryBlock<Allocator_> Image<PixelType_, Allocator_>::relinquish_data_ownership()
{
  const auto ptr = this->byte_ptr();
  const auto len = this->total_bytes();

  mem_view().clear();
  return construct_memory_block_from_existing_memory<Allocator>(ptr, static_cast<std::size_t>(len));
}

template <typename PixelType_, typename Allocator_>
template <typename Derived>
void Image<PixelType_, Allocator_>::copy_rows_from(const ImageBase<Derived>& src)
{
  SELENE_ASSERT(data() && src.data());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < mem_view().height(); ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType_, typename Allocator_>
ImageView<PixelType_, ImageModifiability::Mutable> Image<PixelType_, Allocator_>::allocate_memory(TypedLayout layout)
{
  const auto stride_bytes = std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width));
  const auto nr_bytes_to_allocate = static_cast<std::size_t>(stride_bytes * layout.height);

  auto* memory = mem_alloc().allocate(nr_bytes_to_allocate);
  return ImageView<PixelType, ImageModifiability::Mutable>{{memory},
                                                           {layout.width, layout.height, stride_bytes}};
}

template <typename PixelType_, typename Allocator_>
void Image<PixelType_, Allocator_>::deallocate_memory()
{
  std::uint8_t* ptr = mem_view().byte_ptr();
  mem_alloc().deallocate(ptr, static_cast<std::size_t>(this->total_bytes()));
}

// -----

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool operator==(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1)
{
  return equal(img_0.view(), img_1.view());
}

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool operator!=(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1)
{
  return !(img_0 == img_1);
}

template <typename PixelType0, typename Allocator0, typename PixelType1, typename Allocator1>
bool equal(const Image<PixelType0, Allocator0>& img_0, const Image<PixelType1, Allocator1>& img_1)
{
  return equal(img_0.view(), img_1.view());
}

template <typename PixelType0, typename PixelType1, typename Allocator, ImageModifiability modifiability>
bool equal(const Image<PixelType0, Allocator>& img_0, const ImageView<PixelType1, modifiability>& img_view_1)
{
  return equal(img_0.view(), img_view_1);
}

template <typename PixelType0, typename PixelType1, typename Allocator, ImageModifiability modifiability>
bool equal(const ImageView<PixelType0, modifiability>& img_view_0, const Image<PixelType1, Allocator>& img_1)
{
  return equal(img_view_0, img_1.view());
}

template <typename PixelType, typename Allocator>
void swap(Image<PixelType, Allocator>& img_l, Image<PixelType, Allocator>& img_r) noexcept
{
  using std::swap;
  swap(img_l.view_and_alloc_, img_r.view_and_alloc_);
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_HPP

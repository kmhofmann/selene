// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP
#define SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>
#include <selene/base/_impl/CompressedPair.hpp>

#include <selene/img/dynamic/DynImageView.hpp>

#include <algorithm>

namespace sln {

/// \addtogroup group-img-dynamic
/// @{

template <typename Allocator_ = default_bytes_allocator>
class DynImage;

template <typename Allocator0, typename Allocator1>
bool operator==(const DynImage<Allocator0>& img0, const DynImage<Allocator1>& img1);

template <typename Allocator0, typename Allocator1>
bool operator!=(const DynImage<Allocator0>& img0, const DynImage<Allocator1>& img1);

template <typename Allocator0, typename Allocator1>
inline bool equal(const DynImage<Allocator0>& dyn_img_0, const DynImage<Allocator1>& dyn_img_1);

template <ImageModifiability modifiability, typename Allocator>
bool equal(const DynImage<Allocator>& dyn_img_0, const DynImageView<modifiability>& dyn_img_view_1);

template <ImageModifiability modifiability, typename Allocator>
bool equal(const DynImageView<modifiability>& dyn_img_view_0, const DynImage<Allocator>& dyn_img_1);

template <typename Allocator>
void swap(DynImage<Allocator>& dyn_img_l, DynImage<Allocator>& dyn_img_r) noexcept;

/** \brief Dynamically typed image class.
 *
 * An instance of `DynImage` represents a dynamically typed image with pixel elements in interleaved storage.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * Each image pixel can have an arbitrary number of channels, and each channel/sample in a pixel can have an arbitrary
 * number of bytes.
 *
 * Optionally, an image can have be tagged with a particular `PixelFormat` or a particular `SampleType`.
 * This is mostly a semantic tag and has little influence on the data content.
 *
 * The memory of a `DynImage` instance is always owned by the instance.
 * To express a non-owning relation to the underlying data, use a `DynImageView<modifiability>`.
 */
template <typename Allocator_>
class DynImage
{
public:
  using Allocator = Allocator_;
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  template <typename PixelType> using iterator = typename DynImageView<ImageModifiability::Mutable>::iterator<PixelType>;  ///< The iterator type.
  template <typename PixelType> using const_iterator = typename DynImageView<ImageModifiability::Mutable>::const_iterator<PixelType>;  ///< The const_iterator type.

  constexpr static bool is_view = false;
  constexpr static bool is_owning = true;
  constexpr static bool is_modifiable = true;

  constexpr static ImageModifiability modifiability()
  {
    return ImageModifiability::Mutable;
  }

  DynImage() = default;  ///< Default constructor.

  explicit DynImage(const Allocator& alloc);

  explicit DynImage(UntypedLayout layout,
                    UntypedImageSemantics semantics = UntypedImageSemantics{});

  explicit DynImage(UntypedLayout layout,
                    UntypedImageSemantics semantics,
                    const Allocator& alloc);

  DynImage(std::uint8_t* memory,
           UntypedLayout layout,
           UntypedImageSemantics semantics = UntypedImageSemantics{},
           const Allocator& alloc = Allocator{});

  ~DynImage();

  DynImage(const DynImage<Allocator>&);
  DynImage& operator=(const DynImage<Allocator>&);

  DynImage(DynImage<Allocator>&&) noexcept;
  DynImage<Allocator>& operator=(DynImage<Allocator>&&) noexcept;

  template <ImageModifiability modifiability> explicit DynImage(const DynImageView<modifiability>&,
                                                                const Allocator& alloc = Allocator());

  template <ImageModifiability modifiability> DynImage& operator=(const DynImageView<modifiability>&);

  const UntypedLayout& layout() const noexcept;
  const UntypedImageSemantics& semantics() const noexcept;

  PixelLength width() const noexcept;
  PixelLength height() const noexcept;
  std::int16_t nr_channels() const noexcept;
  std::int16_t nr_bytes_per_channel() const noexcept;
  Stride stride_bytes() const noexcept;
  std::ptrdiff_t row_bytes() const noexcept;
  std::ptrdiff_t total_bytes() const noexcept;
  PixelFormat pixel_format() const noexcept;
  SampleFormat sample_format() const noexcept;

  bool is_packed() const noexcept;
  bool is_empty() const noexcept;
  bool is_valid() const noexcept;

  template <typename PixelType> auto begin() noexcept -> iterator<PixelType>;
  template <typename PixelType> auto begin() const noexcept -> const_iterator<PixelType>;
  template <typename PixelType> auto cbegin() const noexcept -> const_iterator<PixelType>;

  template <typename PixelType> auto end() noexcept -> iterator<PixelType>;
  template <typename PixelType> auto end() const noexcept -> const_iterator<PixelType>;
  template <typename PixelType> auto cend() const noexcept -> const_iterator<PixelType>;

  DataPtrType byte_ptr() noexcept;
  ConstDataPtrType byte_ptr() const noexcept;

  DataPtrType byte_ptr(PixelIndex y) noexcept;
  ConstDataPtrType byte_ptr(PixelIndex y) const noexcept;

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept;
  ConstDataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType> PixelType* data() noexcept;
  template <typename PixelType> const PixelType* data() const noexcept;

  template <typename PixelType> PixelType* data(PixelIndex y) noexcept;
  template <typename PixelType> const PixelType* data(PixelIndex y) const noexcept;

  template <typename PixelType> PixelType* data_row_end(PixelIndex y) noexcept;
  template <typename PixelType> const PixelType* data_row_end(PixelIndex y) const noexcept;

  template <typename PixelType> PixelType* data(PixelIndex x, PixelIndex y) noexcept;
  template <typename PixelType> const PixelType* data(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType> PixelType& pixel(PixelIndex x, PixelIndex y) noexcept;
  template <typename PixelType> const PixelType& pixel(PixelIndex x, PixelIndex y) const noexcept;

  DynImageView<ImageModifiability::Mutable>& view() noexcept;
  DynImageView<ImageModifiability::Constant> view() const noexcept;
  DynImageView<ImageModifiability::Constant> constant_view() const noexcept;

  void clear();

  bool reallocate(UntypedLayout layout,
                  UntypedImageSemantics semantics = UntypedImageSemantics{});

  MemoryBlock<Allocator> relinquish_data_ownership();

private:
//  DynImageView<ImageModifiability::Mutable> view_;
  impl::CompressedPair<DynImageView<ImageModifiability::Mutable>, Allocator> view_and_alloc_;

  DynImageView<ImageModifiability::Mutable>& mem_view() { return view_and_alloc_.first(); }
  const DynImageView<ImageModifiability::Mutable>& mem_view() const { return view_and_alloc_.first(); }

  Allocator& mem_alloc() { return view_and_alloc_.second(); }
  const Allocator& mem_alloc() const { return view_and_alloc_.second(); }

  void copy_rows_from(const DynImage& src);

  DynImageView<ImageModifiability::Mutable> allocate_memory(
      UntypedLayout layout,
      UntypedImageSemantics semantics);

  void deallocate_memory();

  friend void swap<Allocator>(DynImage<Allocator>& dyn_img_l, DynImage<Allocator>& dyn_img_r) noexcept;
};

/// @}

// ----------
// Implementation:

template <typename Allocator_>
DynImage<Allocator_>::DynImage(const Allocator& alloc)
    : view_and_alloc_(DynImageView<ImageModifiability::Mutable>{}, alloc)
{
}

template <typename Allocator_>
DynImage<Allocator_>::DynImage(UntypedLayout layout, UntypedImageSemantics semantics)
    : view_and_alloc_(this->allocate_memory(layout, semantics), Allocator{})
{
}

/** \brief Constructs a dynamic image with the specified layout and pixel semantics.
 *
 * @param layout The image layout.
 * @param semantics The pixel semantics.
 */
template <typename Allocator_>
DynImage<Allocator_>::DynImage(UntypedLayout layout, UntypedImageSemantics semantics, const Allocator& alloc)
    : view_and_alloc_(this->allocate_memory(layout, semantics), alloc)
{
}

/** \brief Constructs a dynamic image with the specified layout and pixel semantics, from an existing block of memory.
 *
 * On construction, the memory will be owned by the image instance.
 *
 * @param memory The memory block representing the image.
 * @param layout The image layout.
 * @param semantics The pixel semantics.
 */
template <typename Allocator_>
DynImage<Allocator_>::DynImage(
    std::uint8_t* memory,
    UntypedLayout layout,
    UntypedImageSemantics semantics,
    const Allocator& alloc)
    : view_and_alloc_({memory, layout, semantics}, alloc)
{
}

/** \brief Destructor.
 *
 * All owned memory will be deallocated.
 */
template <typename Allocator_>
DynImage<Allocator_>::~DynImage()
{
  this->deallocate_memory();
}

/** \brief Copy constructor.
 *
 * @param other The image to be copied from.
 */
template <typename Allocator_>
DynImage<Allocator_>::DynImage(const DynImage<Allocator>& other)
    : view_and_alloc_(allocate_memory(other.layout(),
                                      other.semantics()),
                      other.mem_alloc())
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator.
 *
 * @param other The image to be assigned from.
 * @return A reference to this image.
 */
template <typename Allocator_>
DynImage<Allocator_>& DynImage<Allocator_>::operator=(const DynImage<Allocator>& other)
{
  // Check for self-assignment
  if (this == & other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    mem_view() = allocate_memory(other.layout(), other.semantics());

    mem_alloc() = other.mem_alloc();
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Move constructor.
 *
 * @param other The image to be moved from.
 */
template <typename Allocator_>
DynImage<Allocator_>::DynImage(DynImage<Allocator>&& other) noexcept
    : view_and_alloc_(other.view_and_alloc_)
{
  other.mem_view() = DynImageView<ImageModifiability::Mutable>{{nullptr}, UntypedLayout{}, UntypedImageSemantics{}};
}

/** \brief Move assignment operator.
 *
 * @param other The image to be move-assigned from.
 * @return A reference to this image.
 */
template <typename Allocator_>
DynImage<Allocator_>& DynImage<Allocator_>::operator=(DynImage<Allocator>&& other) noexcept
{
  // Check for self-assignment
  if (this == & other)
  {
    return * this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_and_alloc_ = other.view_and_alloc_;
  other.mem_view() = DynImageView<ImageModifiability::Mutable>{{nullptr}, UntypedLayout{}, UntypedImageSemantics{}};

  return * this;
}

/** \brief Copy constructor taking a `DynImageView` of arbitrary modifiability.
 *
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <typename Allocator_>
template <ImageModifiability modifiability_>
DynImage<Allocator_>::DynImage(const DynImageView<modifiability_>& other, const Allocator& alloc)
    : view_and_alloc_(allocate_memory(other.layout(), other.semantics()), alloc)
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator taking a `DynImageView` of arbitrary modifiability.
 *
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <typename Allocator_>
template <ImageModifiability modifiability_>
DynImage<Allocator_>& DynImage<Allocator_>::operator=(const DynImageView<modifiability_>& other)
{
  // Check for self-assignment
  if (& this->mem_view() == & other)
  {
    return * this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    mem_view() = allocate_memory(other.layout(), other.semantics());
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Returns the dynamic image layout.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The untyped image layout.
 */
template <typename Allocator_>
const UntypedLayout& DynImage<Allocator_>::layout() const noexcept
{
  return mem_view().layout();
}

/** \brief Returns the pixel semantics for the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel semantics.
 */
template <typename Allocator_>
const UntypedImageSemantics& DynImage<Allocator_>::semantics() const noexcept
{
  return mem_view().semantics();
}

/** \brief Returns the image width.
 *
 * @return The image width.
 */
template <typename Allocator_>
PixelLength DynImage<Allocator_>::width() const noexcept
{
  return mem_view().width();
}

/** \brief Returns the image height.
 *
 * @return The image height.
 */
template <typename Allocator_>
PixelLength DynImage<Allocator_>::height() const noexcept
{
  return mem_view().height();
}

/** \brief Returns the number of channels for the image.
 *
 * @return The number of channels.
 */
template <typename Allocator_>
std::int16_t DynImage<Allocator_>::nr_channels() const noexcept
{
  return mem_view().nr_channels();
}

/** \brief Returns the number of bytes per channel for the image.
 *
 * @return The number of bytes per channel.
 */
template <typename Allocator_>
std::int16_t DynImage<Allocator_>::nr_bytes_per_channel() const noexcept
{
  return mem_view().nr_bytes_per_channel();
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * nr_channels() * nr_bytes_per_channel())`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @return The row stride of the image in bytes.
 */
template <typename Allocator_>
Stride DynImage<Allocator_>::stride_bytes() const noexcept
{
  return mem_view().stride_bytes();
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * nr_channels() * nr_bytes_per_channel())`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @return The number of data bytes occupied by each image row.
 */
template <typename Allocator_>
std::ptrdiff_t DynImage<Allocator_>::row_bytes() const noexcept
{
  return mem_view().row_bytes();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @return The total number of bytes occupied by the image data in memory.
 */
template <typename Allocator_>
std::ptrdiff_t DynImage<Allocator_>::total_bytes() const noexcept
{
  return mem_view().total_bytes();
}

/** \brief Returns the specified pixel format of the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel format.
 */
template <typename Allocator_>
PixelFormat DynImage<Allocator_>::pixel_format() const noexcept
{
  return mem_view().pixel_format();
}

/** \brief Returns the specified sample format of the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The sample format.
 */
template <typename Allocator_>
SampleFormat DynImage<Allocator_>::sample_format() const noexcept
{
  return mem_view().sample_format();
}

/** \brief Returns whether the image view is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * nr_channels() * nr_bytes_per_channel())`.
 *
 * @return True, if the image view data is stored packed; false otherwise.
 */
template <typename Allocator_>
bool DynImage<Allocator_>::is_packed() const noexcept
{
  return mem_view().is_packed();
}

/** \brief Returns whether the image is empty.
 *
 * An image [view] is considered empty if its internal data pointer points to `nullptr`, `width() == 0`,
 * `height() == 0`, or any combination of these.
 *
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename Allocator_>
bool DynImage<Allocator_>::is_empty() const noexcept
{
  return mem_view().is_empty();
}

/** \brief Returns whether the image is valid.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @return True, if the image is valid; false otherwise.
 */
template <typename Allocator_>
bool DynImage<Allocator_>::is_valid() const noexcept
{
  return mem_view().is_valid();
}

/** \brief Returns an iterator to the first row.
 *
 * @return Iterator to the first image row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::begin() noexcept -> iterator<PixelType>
{
  return mem_view().template begin<PixelType>();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::begin() const noexcept -> const_iterator<PixelType>
{
  return mem_view().template begin<PixelType>();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::cbegin() const noexcept -> const_iterator<PixelType>
{
  return mem_view().template cbegin<PixelType>();
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Iterator to the image row after the last row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::end() noexcept -> iterator<PixelType>
{
  return mem_view().template end<PixelType>();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::end() const noexcept -> const_iterator<PixelType>
{
  return mem_view().template end<PixelType>();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename Allocator_>
template <typename PixelType>
auto DynImage<Allocator_>::cend() const noexcept -> const_iterator<PixelType>
{
  return mem_view().template cend<PixelType>();
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @return Pointer to the first image data byte.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr() noexcept -> DataPtrType
{
  return mem_view().byte_ptr();
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @return Constant pointer to the first image data byte.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr() const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr();
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr(PixelIndex y) noexcept -> DataPtrType
{
  return mem_view().byte_ptr(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr(PixelIndex y) const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr(PixelIndex x, PixelIndex y) noexcept -> DataPtrType
{
  return mem_view().byte_ptr(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename Allocator_>
auto DynImage<Allocator_>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept -> ConstDataPtrType
{
  return mem_view().byte_ptr(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename Allocator_>
template <typename PixelType>
PixelType* DynImage<Allocator_>::data() noexcept
{
  return mem_view().template data<PixelType>();
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename Allocator_>
template <typename PixelType>
const PixelType* DynImage<Allocator_>::data() const noexcept
{
  return mem_view().template data<PixelType>();
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
PixelType* DynImage<Allocator_>::data(PixelIndex y) noexcept
{
  return mem_view().template data<PixelType>(y);
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
const PixelType* DynImage<Allocator_>::data(PixelIndex y) const noexcept
{
  return mem_view().template data<PixelType>(y);
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
PixelType* DynImage<Allocator_>::data_row_end(PixelIndex y) noexcept
{
  return mem_view().template data_row_end<PixelType>(y);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
const PixelType* DynImage<Allocator_>::data_row_end(PixelIndex y) const noexcept
{
  return mem_view().template data_row_end<PixelType>(y);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
PixelType* DynImage<Allocator_>::data(PixelIndex x, PixelIndex y) noexcept
{
  return mem_view().template data<PixelType>(x, y);
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename Allocator_>
template <typename PixelType>
const PixelType* DynImage<Allocator_>::data(PixelIndex x, PixelIndex y) const noexcept
{
  return mem_view().template data<PixelType>(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename Allocator_>
template <typename PixelType>
PixelType& DynImage<Allocator_>::pixel(PixelIndex x, PixelIndex y) noexcept
{
  return mem_view().template pixel<PixelType>(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename Allocator_>
template <typename PixelType>
const PixelType& DynImage<Allocator_>::pixel(PixelIndex x, PixelIndex y) const noexcept
{
  return mem_view().template pixel<PixelType>(x, y);
}

/** \brief Returns the underlying (mutable) dynamic image view.
 *
 * @return The underlying (mutable) dynamic image view.
 */
template <typename Allocator_>
DynImageView<ImageModifiability::Mutable>& DynImage<Allocator_>::view() noexcept
{
  return mem_view().view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @return A constant image view.
 */
template <typename Allocator_>
DynImageView<ImageModifiability::Constant> DynImage<Allocator_>::view() const noexcept
{
  return mem_view().constant_view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @return A constant image view.
 */
template <typename Allocator_>
DynImageView<ImageModifiability::Constant> DynImage<Allocator_>::constant_view() const noexcept
{
  return mem_view().constant_view();
}

/** \brief Clears the dynamic image; i.e. resets the internal state to the image state after default construction.
 *
 * All allocated memory will be deallocated.
 */
template <typename Allocator_>
void DynImage<Allocator_>::clear()
{
  deallocate_memory();
  mem_view().clear();
}

/** \brief Reallocates the image data according to the specified layout and alignment.
 *
 * @param layout The layout for reallocation.
 * @param row_alignment_bytes The desired row alignment in bytes.
 * @param semantics The new image semantics.
 * @param shrink_to_fit If true, then a reallocation will also take place if the required memory is less than already
 *                      allocated. If false, then the already allocated memory will be kept, if the required memory is
 *                      less than already allocated.
 * @return True, if a memory reallocation took place; false otherwise.
 */
template <typename Allocator_>
bool DynImage<Allocator_>::reallocate(UntypedLayout layout, UntypedImageSemantics semantics)
{
  if (layout == this->mem_view().layout())
  {
    return false;
  }

  layout.stride_bytes = std::max(layout.stride_bytes, Stride{layout.nr_bytes_per_channel * layout.nr_channels * layout.width});

  this->deallocate_memory();
  mem_view() = this->allocate_memory(layout, semantics);
  return true;
}

/** \brief Releases the owned memory from the dynamic image instance and returns it.
 *
 * As a result, the dynamic image will be empty, and no memory will be owned.
 *
 * @return The owned memory block.
 */
template <typename Allocator_>
MemoryBlock<Allocator_> DynImage<Allocator_>::relinquish_data_ownership()
{
  const auto ptr = this->byte_ptr();
  const auto len = this->total_bytes();

  mem_view().clear();
  return construct_memory_block_from_existing_memory<Allocator>(ptr, static_cast<std::size_t>(len));
}

template <typename Allocator_>
void DynImage<Allocator_>::copy_rows_from(const DynImage& src)
{
  SELENE_ASSERT(byte_ptr() && src.byte_ptr());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < mem_view().height(); ++y)
  {
    std::copy(src.byte_ptr(y), src.byte_ptr(y) + src.row_bytes(), byte_ptr(y));
  }
}

template <typename Allocator_>
DynImageView<ImageModifiability::Mutable> DynImage<Allocator_>::allocate_memory(
    UntypedLayout layout,
    UntypedImageSemantics semantics)
{
  const auto stride_bytes = std::max(layout.stride_bytes, Stride{layout.nr_bytes_per_channel * layout.nr_channels * layout.width});
  const auto nr_bytes_to_allocate = static_cast<std::size_t>(stride_bytes * layout.height);

  auto* memory = mem_alloc().allocate(nr_bytes_to_allocate);

  return DynImageView<ImageModifiability::Mutable>{
      {memory},
      {layout.width, layout.height, layout.nr_channels, layout.nr_bytes_per_channel, stride_bytes},
      semantics};
}

template <typename Allocator_>
void DynImage<Allocator_>::deallocate_memory()
{
  std::uint8_t* ptr = mem_view().byte_ptr();
  const auto nr_bytes_to_deallocate = this->total_bytes();
  mem_alloc().deallocate(ptr, static_cast<std::size_t>(nr_bytes_to_deallocate));
}

// -----

template <typename Allocator0, typename Allocator1>
bool operator==(const DynImage<Allocator0>& img0, const DynImage<Allocator1>& img1)
{
  return equal(img0.view(), img1.view());
}

template <typename Allocator0, typename Allocator1>
bool operator!=(const DynImage<Allocator0>& img0, const DynImage<Allocator1>& img1)
{
  return !(img0 == img1);
}

template <typename Allocator0, typename Allocator1>
inline bool equal(const DynImage<Allocator0>& dyn_img_0, const DynImage<Allocator1>& dyn_img_1)
{
  return equal(dyn_img_0.view(), dyn_img_1.view());
}

template <ImageModifiability modifiability, typename Allocator>
bool equal(const DynImage<Allocator>& dyn_img_0, const DynImageView<modifiability>& dyn_img_view_1)
{
  return equal(dyn_img_0.view(), dyn_img_view_1);
}

template <ImageModifiability modifiability, typename Allocator>
bool equal(const DynImageView<modifiability>& dyn_img_view_0, const DynImage<Allocator>& dyn_img_1)
{
  return equal(dyn_img_view_0, dyn_img_1.view());
}

template <typename Allocator>
void swap(DynImage<Allocator>& dyn_img_l, DynImage<Allocator>& dyn_img_r) noexcept
{
  using std::swap;
  swap(dyn_img_l.view_and_alloc_, dyn_img_r.view_and_alloc_);
}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP
#define SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/dynamic/DynImageView.hpp>

#include <algorithm>

namespace sln {

class DynImage;

bool operator==(const DynImage& img0, const DynImage& img1);

bool operator!=(const DynImage& img0, const DynImage& img1);

inline bool equal(const DynImage& dyn_img_0, const DynImage& dyn_img_1);

template <ImageModifiability modifiability>
bool equal(const DynImage& dyn_img_0, const DynImageView<modifiability>& dyn_img_view_1);

template <ImageModifiability modifiability>
bool equal(const DynImageView<modifiability>& dyn_img_view_0, const DynImage& dyn_img_1);

void swap(DynImage& dyn_img_l, DynImage& dyn_img_r) noexcept;

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
class DynImage
{
public:
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  template <typename PixelType> using iterator = DynImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The iterator type.
  template <typename PixelType> using const_iterator = ConstDynImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The const_iterator type.

  constexpr static bool is_view = false;
  constexpr static bool is_owning = true;
  constexpr static bool is_modifiable = true;

  constexpr static ImageModifiability modifiability()
  {
    return ImageModifiability::Mutable;
  }

  DynImage() = default;  ///< Default constructor.

  explicit DynImage(UntypedLayout layout, UntypedImageSemantics semantics = UntypedImageSemantics{});

  DynImage(UntypedLayout layout,
           ImageRowAlignment row_alignment_bytes,
           UntypedImageSemantics semantics = UntypedImageSemantics{});

  DynImage(MemoryBlock<AlignedNewAllocator>&& memory,
           UntypedLayout layout,
           UntypedImageSemantics semantics = UntypedImageSemantics{});

  ~DynImage();

  DynImage(const DynImage&);
  DynImage& operator=(const DynImage&);

  DynImage(DynImage&&) noexcept;
  DynImage& operator=(DynImage&&) noexcept;

  template <ImageModifiability modifiability> explicit DynImage(const DynImageView<modifiability>&);

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
                  ImageRowAlignment row_alignment_bytes,
                  UntypedImageSemantics semantics = UntypedImageSemantics{},
                  bool shrink_to_fit = true);

  MemoryBlock<AlignedNewAllocator> relinquish_data_ownership();

private:
  constexpr static auto default_base_alignment_bytes = ImageRowAlignment{16ul};

  DynImageView<ImageModifiability::Mutable> view_;

  void copy_rows_from(const DynImage& src);

  DynImageView<ImageModifiability::Mutable> allocate_memory(
      UntypedLayout layout,
      std::ptrdiff_t base_alignment_bytes,
      std::ptrdiff_t row_alignment_bytes,
      UntypedImageSemantics semantics);

  void deallocate_memory();

  friend void swap(DynImage& dyn_img_l, DynImage& dyn_img_r) noexcept;
};

// ----------
// Implementation:

/** \brief Constructs a dynamic image with the specified layout and pixel semantics.
 *
 * @param layout The image layout.
 * @param semantics The pixel semantics.
 */
inline DynImage::DynImage(UntypedLayout layout, UntypedImageSemantics semantics)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, 0, semantics))
{
}

/** \brief Constructs a dynamic image with the specified layout, row alignment, and pixel semantics.
 *
 * @param layout The image layout.
 * @param row_alignment_bytes The row alignment in bytes.
 * @param semantics The pixel semantics.
 */
inline DynImage::DynImage(UntypedLayout layout, ImageRowAlignment row_alignment_bytes, UntypedImageSemantics semantics)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes, semantics))
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
inline DynImage::DynImage(
    MemoryBlock<AlignedNewAllocator>&& memory,
    UntypedLayout layout,
    UntypedImageSemantics semantics)
    : view_(memory.transfer_data(), layout, semantics)
{
}

/** \brief Destructor.
 *
 * All owned memory will be deallocated.
 */
inline DynImage::~DynImage()
{
  this->deallocate_memory();
}

/** \brief Copy constructor.
 *
 * @param other The image to be copied from.
 */
inline DynImage::DynImage(const DynImage& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()),
                            other.semantics()))
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator.
 *
 * @param other The image to be assigned from.
 * @return A reference to this image.
 */
inline DynImage& DynImage::operator=(const DynImage& other)
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
    view_ = allocate_memory(
        other.layout(), default_base_alignment_bytes,
        impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()), other.stride_bytes()),
        other.semantics());
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Move constructor.
 *
 * @param other The image to be moved from.
 */
inline DynImage::DynImage(DynImage&& other) noexcept
    : view_(other.view_)
{
  other.view_ = DynImageView<ImageModifiability::Mutable>{{nullptr}, UntypedLayout{}, UntypedImageSemantics{}};
}

/** \brief Move assignment operator.
 *
 * @param other The image to be move-assigned from.
 * @return A reference to this image.
 */
inline DynImage& DynImage::operator=(DynImage&& other) noexcept
{
  // Check for self-assignment
  if (this == & other)
  {
    return * this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_ = other.view_;
  other.view_ = DynImageView<ImageModifiability::Mutable>{{nullptr}, UntypedLayout{}, UntypedImageSemantics{}};

  return * this;
}

/** \brief Copy constructor taking a `DynImageView` of arbitrary modifiability.
 *
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <ImageModifiability modifiability_>
DynImage::DynImage(const DynImageView<modifiability_>& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()),
                            other.semantics()))
{
  copy_rows_from(other);
}

/** \brief Copy assignment operator taking a `DynImageView` of arbitrary modifiability.
 *
 * @tparam modifiability_ The modifiability value of the other image.
 * @param other The image to be copied from.
 */
template <ImageModifiability modifiability_>
DynImage& DynImage::operator=(const DynImageView<modifiability_>& other)
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
    view_ = allocate_memory(
        other.layout(), default_base_alignment_bytes,
        impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()), other.stride_bytes()),
        other.semantics());
  }

  copy_rows_from(other);

  return * this;
}

/** \brief Returns the dynamic image layout.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The untyped image layout.
 */
inline const UntypedLayout& DynImage::layout() const noexcept
{
  return view_.layout();
}

/** \brief Returns the pixel semantics for the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel semantics.
 */
inline const UntypedImageSemantics& DynImage::semantics() const noexcept
{
  return view_.semantics();
}

/** \brief Returns the image width.
 *
 * @return The image width.
 */
inline PixelLength DynImage::width() const noexcept
{
  return view_.width();
}

/** \brief Returns the image height.
 *
 * @return The image height.
 */
inline PixelLength DynImage::height() const noexcept
{
  return view_.height();
}

/** \brief Returns the number of channels for the image.
 *
 * @return The number of channels.
 */
inline std::int16_t DynImage::nr_channels() const noexcept
{
  return view_.nr_channels();
}

/** \brief Returns the number of bytes per channel for the image.
 *
 * @return The number of bytes per channel.
 */
inline std::int16_t DynImage::nr_bytes_per_channel() const noexcept
{
  return view_.nr_bytes_per_channel();
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
inline Stride DynImage::stride_bytes() const noexcept
{
  return view_.stride_bytes();
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * nr_channels() * nr_bytes_per_channel())`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @return The number of data bytes occupied by each image row.
 */
inline std::ptrdiff_t DynImage::row_bytes() const noexcept
{
  return view_.row_bytes();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @return The total number of bytes occupied by the image data in memory.
 */
inline std::ptrdiff_t DynImage::total_bytes() const noexcept
{
  return view_.total_bytes();
}

/** \brief Returns the specified pixel format of the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel format.
 */
inline PixelFormat DynImage::pixel_format() const noexcept
{
  return view_.pixel_format();
}

/** \brief Returns the specified sample format of the dynamic image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The sample format.
 */
inline SampleFormat DynImage::sample_format() const noexcept
{
  return view_.sample_format();
}

/** \brief Returns whether the image view is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * nr_channels() * nr_bytes_per_channel())`.
 *
 * @return True, if the image view data is stored packed; false otherwise.
 */
inline bool DynImage::is_packed() const noexcept
{
  return view_.is_packed();
}

/** \brief Returns whether the image is empty.
 *
 * An image [view] is considered empty if its internal data pointer points to `nullptr`, `width() == 0`,
 * `height() == 0`, or any combination of these.
 *
 * @return True, if the image is empty; false if it is non-empty.
 */
inline bool DynImage::is_empty() const noexcept
{
  return view_.is_empty();
}

/** \brief Returns whether the image is valid.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @return True, if the image is valid; false otherwise.
 */
inline bool DynImage::is_valid() const noexcept
{
  return view_.is_valid();
}

/** \brief Returns an iterator to the first row.
 *
 * @return Iterator to the first image row.
 */
template <typename PixelType>
auto DynImage::begin() noexcept -> iterator<PixelType>
{
  return view_.begin<PixelType>();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType>
auto DynImage::begin() const noexcept -> const_iterator<PixelType>
{
  return view_.begin<PixelType>();
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <typename PixelType>
auto DynImage::cbegin() const noexcept -> const_iterator<PixelType>
{
  return view_.cbegin<PixelType>();
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Iterator to the image row after the last row.
 */
template <typename PixelType>
auto DynImage::end() noexcept -> iterator<PixelType>
{
  return view_.end<PixelType>();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType>
auto DynImage::end() const noexcept -> const_iterator<PixelType>
{
  return view_.end<PixelType>();
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <typename PixelType>
auto DynImage::cend() const noexcept -> const_iterator<PixelType>
{
  return view_.cend<PixelType>();
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @return Pointer to the first image data byte.
 */
inline auto DynImage::byte_ptr() noexcept -> DataPtrType
{
  return view_.byte_ptr();
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @return Constant pointer to the first image data byte.
 */
inline auto DynImage::byte_ptr() const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr();
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
inline auto DynImage::byte_ptr(PixelIndex y) noexcept -> DataPtrType
{
  return view_.byte_ptr(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
inline auto DynImage::byte_ptr(PixelIndex y) const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
inline auto DynImage::byte_ptr(PixelIndex x, PixelIndex y) noexcept -> DataPtrType
{
  return view_.byte_ptr(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
inline auto DynImage::byte_ptr(PixelIndex x, PixelIndex y) const noexcept -> ConstDataPtrType
{
  return view_.byte_ptr(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename PixelType>
PixelType* DynImage::data() noexcept
{
  return view_.data<PixelType>();
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename PixelType>
const PixelType* DynImage::data() const noexcept
{
  return view_.data<PixelType>();
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
PixelType* DynImage::data(PixelIndex y) noexcept
{
  return view_.data<PixelType>(y);
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
const PixelType* DynImage::data(PixelIndex y) const noexcept
{
  return view_.data<PixelType>(y);
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
PixelType* DynImage::data_row_end(PixelIndex y) noexcept
{
  return view_.data_row_end<PixelType>(y);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
const PixelType* DynImage::data_row_end(PixelIndex y) const noexcept
{
  return view_.data_row_end<PixelType>(y);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
PixelType* DynImage::data(PixelIndex x, PixelIndex y) noexcept
{
  return view_.data<PixelType>(x, y);
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
const PixelType* DynImage::data(PixelIndex x, PixelIndex y) const noexcept
{
  return view_.data<PixelType>(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
PixelType& DynImage::pixel(PixelIndex x, PixelIndex y) noexcept
{
  return view_.pixel<PixelType>(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
const PixelType& DynImage::pixel(PixelIndex x, PixelIndex y) const noexcept
{
  return view_.pixel<PixelType>(x, y);
}

/** \brief Returns the underlying (mutable) dynamic image view.
 *
 * @return The underlying (mutable) dynamic image view.
 */
inline DynImageView<ImageModifiability::Mutable>& DynImage::view() noexcept
{
  return view_.view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @return A constant image view.
 */
inline DynImageView<ImageModifiability::Constant> DynImage::view() const noexcept
{
  return view_.constant_view();
}

/** \brief Returns a constant image view on the underlying data.
 *
 * @return A constant image view.
 */
inline DynImageView<ImageModifiability::Constant> DynImage::constant_view() const noexcept
{
  return view_.constant_view();
}

/** \brief Clears the dynamic image; i.e. resets the internal state to the image state after default construction.
 *
 * All allocated memory will be deallocated.
 */
inline void DynImage::clear()
{
  deallocate_memory();
  view_.clear();
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
inline bool DynImage::reallocate(
    UntypedLayout layout,
    ImageRowAlignment row_alignment_bytes,
    UntypedImageSemantics semantics,
    bool shrink_to_fit)
{
  if (layout == this->view_.layout())
  {
    return false;
  }

  layout.stride_bytes = impl::compute_stride_bytes(
      std::max(layout.stride_bytes, Stride{layout.nr_bytes_per_channel * layout.nr_channels * layout.width}),
      row_alignment_bytes);
  const auto nr_bytes_to_allocate = layout.stride_bytes * layout.height;
  const auto nr_currently_allocated_bytes = this->stride_bytes() * this->height();

  // No need to act if size parameters match
  const auto bytes_match = shrink_to_fit ? (nr_bytes_to_allocate == nr_currently_allocated_bytes)
                                         : (nr_bytes_to_allocate <= nr_currently_allocated_bytes);
  if (bytes_match)
  {
    view_ = DynImageView<ImageModifiability::Mutable>(this->byte_ptr(), layout);
    return false;
  }

  this->deallocate_memory();
  view_ = this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes, semantics);
  return true;
}

/** \brief Releases the owned memory from the dynamic image instance and returns it.
 *
 * As a result, the dynamic image will be empty, and no memory will be owned.
 *
 * @return The owned memory block.
 */
inline MemoryBlock<AlignedNewAllocator> DynImage::relinquish_data_ownership()
{
  const auto ptr = this->byte_ptr();
  const auto len = this->total_bytes();

  view_.clear();
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, static_cast<std::size_t>(len));
}

inline void DynImage::copy_rows_from(const DynImage& src)
{
  SELENE_ASSERT(byte_ptr() && src.byte_ptr());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < view_.height(); ++y)
  {
    std::copy(src.byte_ptr(y), src.byte_ptr(y) + src.row_bytes(), byte_ptr(y));
  }
}

inline DynImageView<ImageModifiability::Mutable> DynImage::allocate_memory(
    UntypedLayout layout,
    std::ptrdiff_t base_alignment_bytes,
    std::ptrdiff_t row_alignment_bytes,
    UntypedImageSemantics semantics)
{
  const auto stride_bytes = impl::compute_stride_bytes(
      std::max(layout.stride_bytes, Stride{layout.nr_bytes_per_channel * layout.nr_channels * layout.width}),
      row_alignment_bytes);
  const auto nr_bytes_to_allocate = stride_bytes * layout.height;

  base_alignment_bytes = std::max(row_alignment_bytes, base_alignment_bytes);
  auto memory = sln::AlignedNewAllocator::allocate(
      static_cast<std::size_t>(nr_bytes_to_allocate),
      static_cast<std::size_t>(base_alignment_bytes));
  SELENE_ASSERT(static_cast<std::ptrdiff_t>(memory.size()) == nr_bytes_to_allocate);

  return DynImageView<ImageModifiability::Mutable>{
      {memory.transfer_data()},
      {layout.width, layout.height, layout.nr_channels, layout.nr_bytes_per_channel, stride_bytes},
      semantics};
}

inline void DynImage::deallocate_memory()
{
  std::uint8_t* ptr = view_.byte_ptr();
  sln::AlignedNewAllocator::deallocate(ptr);
}

// -----

inline bool operator==(const DynImage& img0, const DynImage& img1)
{
  return equal(img0.view(), img1.view());
}

inline bool operator!=(const DynImage& img0, const DynImage& img1)
{
  return !(img0 == img1);
}

inline bool equal(const DynImage& dyn_img_0, const DynImage& dyn_img_1)
{
  return equal(dyn_img_0.view(), dyn_img_1.view());
}

template <ImageModifiability modifiability>
bool equal(const DynImage& dyn_img_0, const DynImageView<modifiability>& dyn_img_view_1)
{
  return equal(dyn_img_0.view(), dyn_img_view_1);
}

template <ImageModifiability modifiability>
bool equal(const DynImageView<modifiability>& dyn_img_view_0, const DynImage& dyn_img_1)
{
  return equal(dyn_img_view_0, dyn_img_1.view());
}

inline void swap(DynImage& dyn_img_l, DynImage& dyn_img_r) noexcept
{
  using std::swap;
  swap(dyn_img_l.view_, dyn_img_r.view_);
}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_DYN_IMAGE_HPP

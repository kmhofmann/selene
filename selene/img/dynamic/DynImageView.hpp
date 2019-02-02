// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_DYN_IMAGE_VIEW_HPP
#define SELENE_IMG_DYNAMIC_DYN_IMAGE_VIEW_HPP

/// @file

#include <selene/img/common/Iterators.hpp>
#include <selene/img/dynamic/UntypedLayout.hpp>

#include <cstring>
#include <iterator>
#include <type_traits>
#include <utility>

namespace sln {

template <ImageModifiability modifiability_ = ImageModifiability::Constant>
class DynImageView;

namespace impl {

// Proxy class for iteration.
template <typename PixelType_, ImageModifiability modifiability, bool is_const>
class TypedDynImageView
{
public:
  using ViewPtr = std::conditional_t<is_const, const DynImageView<modifiability>*, DynImageView<modifiability>*>;
  using PixelType = PixelType_;
  using PixelTypePtr = typename DynImageView<modifiability>::template PixelTypePtr<PixelType>;

  explicit TypedDynImageView(ViewPtr view) : view_(view) { }

  PixelTypePtr data() const noexcept { return view_->template data<PixelType>(); }
  PixelTypePtr data(PixelIndex y) const noexcept { return view_->template data<PixelType>(y); }
  PixelTypePtr data_row_end(PixelIndex y) const noexcept { return view_->template data_row_end<PixelType>(y); }
  PixelTypePtr data(PixelIndex x, PixelIndex y) const noexcept { return view_->template data<PixelType>(x, y); }

  template <bool is_const_2> bool operator==(const TypedDynImageView<PixelType_, modifiability, is_const_2>& v) const noexcept { return view_ == v.view_; }

private:
  ViewPtr view_;
  template <typename PixelType2, ImageModifiability modifiability_2, bool is_const_2> friend class TypedDynImageView;
};

} // namespace impl

/// \addtogroup group-img-dynamic
/// @{

using MutableDynImageView = DynImageView<ImageModifiability::Mutable>;  ///< A dynamic image view pointing to mutable data.
using ConstantDynImageView = DynImageView<ImageModifiability::Constant>;  ///< A dynamic image view pointing to constant data.

template <ImageModifiability modifiability_0, ImageModifiability modifiability_1>
bool equal(const DynImageView<modifiability_0>& dyn_img_0, const DynImageView<modifiability_1>& dyn_img_1);

template <ImageModifiability modifiability_>
void swap(DynImageView<modifiability_>& dyn_img_view_l, DynImageView<modifiability_>& dyn_img_view_r) noexcept;

/** \brief Dynamically typed image view class, i.e. non-owning.
 *
 * An instance of `DynImageView<modifiability>` represents a dynamically typed image view with pixel elements in
 * interleaved storage.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * Each image pixel can have an arbitrary number of channels, and each channel/sample in a pixel can have an arbitrary
 * number of bytes.
 *
 * Optionally, an image can have be tagged with a particular `PixelFormat` or a particular `SampleType`.
 * This is mostly a semantic tag and has little influence on the data content.
 *
 * The memory of a `DynImage` instance is never owned by the instance.
 * To express an owning relation to the underlying data, use a `DynImage`.
 *
 * A view can either be created to point to constant data (`ImageModifiability::Constant`), or to modifiable (mutable)
 * data (`ImageModifiability::Mutable`); this is determined by the respective non-type template parameter.
 *
 * @tparam modifiability_ Expresses whether the view contents can be modified (`ImageModifiability::Mutable`) or not
 *                        (ImageModifiability::Constant`).
 */
template <ImageModifiability modifiability_>
class DynImageView
{
  template <typename PixelType, bool is_const>
    using TypedProxyView = impl::TypedDynImageView<PixelType, modifiability_, is_const>;
  template <typename PixelType> using IteratorRow = DynImageRow<TypedProxyView<PixelType, false>, PixelType, false>;
  template <typename PixelType> using ConstIteratorRow = DynImageRow<TypedProxyView<PixelType, true>, PixelType, true>;

public:
  using DataPtrType = typename DataPtr<modifiability_>::Type;
  template <typename PixelType>
    using PixelTypePtr = std::conditional_t<modifiability_ == ImageModifiability::Constant, const PixelType*, PixelType*>;

  template <typename PixelType> using iterator = ImageRowIterator<TypedProxyView<PixelType, false>,
                                                                  IteratorRow<PixelType>>;  ///< The iterator type.
  template <typename PixelType> using const_iterator = ImageRowIterator<TypedProxyView<PixelType, true>,
                                                                        ConstIteratorRow<PixelType>>;  ///< The const_iterator type.

  constexpr static bool is_view = true;
  constexpr static bool is_owning = false;
  constexpr static bool is_modifiable = (modifiability_ == ImageModifiability::Mutable);

  constexpr static ImageModifiability modifiability()
  {
    return modifiability_;
  }

  DynImageView() = default;  ///< Default constructor.
  DynImageView(DataPtr<modifiability_> ptr,
               UntypedLayout layout,
               UntypedImageSemantics semantics = UntypedImageSemantics{});

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

  template <typename PixelType> iterator<PixelType> begin() noexcept;
  template <typename PixelType> const_iterator<PixelType> begin() const noexcept;
  template <typename PixelType> const_iterator<PixelType> cbegin() const noexcept;

  template <typename PixelType> iterator<PixelType> end() noexcept;
  template <typename PixelType> const_iterator<PixelType> end() const noexcept;
  template <typename PixelType> const_iterator<PixelType> cend() const noexcept;

  DataPtrType byte_ptr() const noexcept;
  DataPtrType byte_ptr(PixelIndex y) const noexcept;
  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType> PixelTypePtr<PixelType> data() const noexcept;
  template <typename PixelType> PixelTypePtr<PixelType> data(PixelIndex y) const noexcept;
  template <typename PixelType> PixelTypePtr<PixelType> data_row_end(PixelIndex y) const noexcept;
  template <typename PixelType> PixelTypePtr<PixelType> data(PixelIndex x, PixelIndex y) const noexcept;

  template <typename PixelType>
  auto pixel(PixelIndex x, PixelIndex y) noexcept
      -> std::conditional_t<modifiability_ == ImageModifiability::Mutable, PixelType&, const PixelType&>;

  template <typename PixelType>
  const PixelType& pixel(PixelIndex x, PixelIndex y) const noexcept;

  DynImageView<modifiability_>& view() noexcept;
  decltype(auto) view() const noexcept;
  decltype(auto) constant_view() const noexcept;

  void clear();

private:
  DataPtr<modifiability_> ptr_;
  UntypedLayout layout_;
  UntypedImageSemantics semantics_;

  std::ptrdiff_t compute_data_offset(PixelIndex y) const noexcept;
  std::ptrdiff_t compute_data_offset(PixelIndex x, PixelIndex y) const noexcept;

  friend void swap<modifiability_>(DynImageView<modifiability_>&, DynImageView<modifiability_>&) noexcept;
};

/// @}

// ----------
// Implementation:

/** \brief Constructs a dynamic image view onto the specified memory region, given the specified layout and pixel
 * semantics.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @param ptr A pointer to the memory block representing the image data
 * @param layout The image layout.
 * @param semantics The pixel semantics.
 */
template <ImageModifiability modifiability_>
DynImageView<modifiability_>::DynImageView(DataPtr<modifiability_> ptr,
                                           UntypedLayout layout,
                                           UntypedImageSemantics semantics)
    : ptr_(ptr), layout_(layout), semantics_(semantics)
{
}

/** \brief Returns the dynamic image view layout.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The untyped image view layout.
 */
template <ImageModifiability modifiability_>
const UntypedLayout& DynImageView<modifiability_>::layout() const noexcept
{
  return layout_;
}

/** \brief Returns the pixel semantics for the dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel semantics.
 */
template <ImageModifiability modifiability_>
const UntypedImageSemantics& DynImageView<modifiability_>::semantics() const noexcept
{
  return semantics_;
}

/** \brief Returns the image view width.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The image view width.
 */
template <ImageModifiability modifiability_>
PixelLength DynImageView<modifiability_>::width() const noexcept
{
  return layout_.width;
}

/** \brief Returns the image view height.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The image view height.
 */
template <ImageModifiability modifiability_>
PixelLength DynImageView<modifiability_>::height() const noexcept
{
  return layout_.height;
}

/** \brief Returns the number of channels for the image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The number of channels.
 */
template <ImageModifiability modifiability_>
std::int16_t DynImageView<modifiability_>::nr_channels() const noexcept
{
  return layout_.nr_channels;
}

/** \brief Returns the number of bytes per channel for the image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The number of bytes per channel.
 */
template <ImageModifiability modifiability_>
std::int16_t DynImageView<modifiability_>::nr_bytes_per_channel() const noexcept
{
  return layout_.nr_bytes_per_channel;
}

/** \brief Returns the row stride of the image view in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * nr_channels() * nr_bytes_per_channel())`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The row stride of the image view in bytes.
 */
template <ImageModifiability modifiability_>
Stride DynImageView<modifiability_>::stride_bytes() const noexcept
{
  return layout_.stride_bytes;
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * nr_channels() * nr_bytes_per_channel())`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The number of data bytes occupied by each image row.
 */
template <ImageModifiability modifiability_>
std::ptrdiff_t DynImageView<modifiability_>::row_bytes() const noexcept
{
  return layout_.row_bytes();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The total number of bytes occupied by the image data in memory.
 */
template <ImageModifiability modifiability_>
std::ptrdiff_t DynImageView<modifiability_>::total_bytes() const noexcept
{
  return layout_.total_bytes();
}

/** \brief Returns the specified pixel format of the dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The pixel format.
 */
template <ImageModifiability modifiability_>
PixelFormat DynImageView<modifiability_>::pixel_format() const noexcept
{
  return semantics_.pixel_format;
}

/** \brief Returns the specified sample format of the dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return The sample format.
 */
template <ImageModifiability modifiability_>
SampleFormat DynImageView<modifiability_>::sample_format() const noexcept
{
  return semantics_.sample_format;
}

/** \brief Returns whether the image view is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * nr_channels() * nr_bytes_per_channel())`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return True, if the image view data is stored packed; false otherwise.
 */
template <ImageModifiability modifiability_>
bool DynImageView<modifiability_>::is_packed() const noexcept
{
  return layout_.is_packed();
}

/** \brief Returns whether the image view is empty.
 *
 * An image [view] is considered empty if its internal data pointer points to `nullptr`, `width() == 0`,
 * `height() == 0`, or any combination of these.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return True, if the image view is empty; false if it is non-empty.
 */
template <ImageModifiability modifiability_>
bool DynImageView<modifiability_>::is_empty() const noexcept
{
  return ptr_.data() == nullptr || layout_.width == 0 || layout_.height == 0;
}

/** \brief Returns whether the image view is valid.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return True, if the image view is valid; false otherwise.
 */
template <ImageModifiability modifiability_>
bool DynImageView<modifiability_>::is_valid() const noexcept
{
  return !is_empty();
}

/** \brief Returns an iterator to the first row.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Iterator to the first image row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::begin() noexcept -> iterator<PixelType>
{
  auto row = IteratorRow<PixelType>(TypedProxyView<PixelType, false>(this), 0_idx);
  return iterator<PixelType>(row);
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::begin() const noexcept -> const_iterator<PixelType>
{
  auto row = ConstIteratorRow<PixelType>(TypedProxyView<PixelType, true>(this), 0_idx);
  return const_iterator<PixelType>(row);
}

/** \brief Returns a constant iterator to the first row.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the first image row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::cbegin() const noexcept -> const_iterator<PixelType>
{
  auto row = ConstIteratorRow<PixelType>(TypedProxyView<PixelType, true>(this), 0_idx);
  return const_iterator<PixelType>(row);
}

/** \brief Returns an iterator to the row after the last row of the image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Iterator to the image row after the last row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::end() noexcept -> iterator<PixelType>
{
  auto row = IteratorRow<PixelType>(TypedProxyView<PixelType, false>(this), PixelIndex{this->height()});
  return iterator<PixelType>(row);
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::end() const noexcept -> const_iterator<PixelType>
{
  auto row = ConstIteratorRow<PixelType>(TypedProxyView<PixelType, true>(this), PixelIndex{this->height()});
  return const_iterator<PixelType>(row);
}

/** \brief Returns a constant iterator to the row after the last row of the image.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Constant iterator to the image row after the last row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::cend() const noexcept -> const_iterator<PixelType>
{
  auto row = ConstIteratorRow<PixelType>(TypedProxyView<PixelType, true>(this), PixelIndex{this->height()});
  return const_iterator<PixelType>(row);
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return Pointer to the first image data byte.
 */
template <ImageModifiability modifiability_>
auto DynImageView<modifiability_>::byte_ptr() const noexcept -> DataPtrType
{
  return ptr_.data();
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <ImageModifiability modifiability_>
auto DynImageView<modifiability_>::byte_ptr(PixelIndex y) const noexcept -> DataPtrType
{
  return ptr_.data() + this->compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <ImageModifiability modifiability_>
auto DynImageView<modifiability_>::byte_ptr(PixelIndex x, PixelIndex y) const noexcept -> DataPtrType
{
  return ptr_.data() + this->compute_data_offset(x, y);
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @return Pointer to the first pixel element.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::data() const noexcept -> PixelTypePtr<PixelType>
{
  using VoidPtrType = std::conditional_t<modifiability_ == ImageModifiability::Constant, const void*, void*>;
  return static_cast<PixelTypePtr<PixelType>>(static_cast<VoidPtrType>(this->byte_ptr()));
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::data(PixelIndex y) const noexcept -> PixelTypePtr<PixelType>
{
  using VoidPtrType = std::conditional_t<modifiability_ == ImageModifiability::Constant, const void*, void*>;
  return static_cast<PixelTypePtr<PixelType>>(static_cast<VoidPtrType>(this->byte_ptr(y)));
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::data_row_end(PixelIndex y) const noexcept -> PixelTypePtr<PixelType>
{
  using VoidPtrType = std::conditional_t<modifiability_ == ImageModifiability::Constant, const void*, void*>;
  return static_cast<PixelTypePtr<PixelType>>(static_cast<VoidPtrType>(
      this->byte_ptr(y) + layout_.nr_bytes_per_pixel() * layout_.width));
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::data(PixelIndex x, PixelIndex y) const noexcept -> PixelTypePtr<PixelType>
{
  using VoidPtrType = std::conditional_t<modifiability_ == ImageModifiability::Constant, const void*, void*>;
  return static_cast<PixelTypePtr<PixelType>>(static_cast<VoidPtrType>(this->byte_ptr(x, y)));
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
auto DynImageView<modifiability_>::pixel(PixelIndex x, PixelIndex y) noexcept
    -> std::conditional_t<modifiability_ == ImageModifiability::Mutable, PixelType&, const PixelType&>
{
  return *data<PixelType>(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <ImageModifiability modifiability_>
template <typename PixelType>
const PixelType& DynImageView<modifiability_>::pixel(PixelIndex x, PixelIndex y) const noexcept
{
  return *data<PixelType>(x, y);
}

/** \brief Returns the dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return Returns the dynamic image view itself; i.e. a reference to `this`.
 */
template <ImageModifiability modifiability_>
DynImageView<modifiability_>& DynImageView<modifiability_>::view() noexcept
{
  return *this;
}

/** \brief Returns a constant dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return A constant image view.
 */
template <ImageModifiability modifiability_>
decltype(auto) DynImageView<modifiability_>::view() const noexcept
{
  return *this;
}

/** \brief Returns a constant dynamic image view.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 * @return A constant image view.
 */
template <ImageModifiability modifiability_>
decltype(auto) DynImageView<modifiability_>::constant_view() const noexcept
{
  if constexpr (is_modifiable)
  {
    return DynImageView<ImageModifiability::Constant>{this->byte_ptr(), this->layout(), this->semantics()};
  }
  else
  {
    return static_cast<const DynImageView<ImageModifiability::Constant>&>(*this);
  }
}

/** \brief Clears the dynamic image view; i.e. resets the internal state to the dynamic image view state after default
 * construction.
 *
 * @tparam modifiability_ Determines whether image contents are constant or mutable.
 */
template <ImageModifiability modifiability_>
void DynImageView<modifiability_>::clear()
{
  ptr_ = DataPtr<modifiability_>{};
  layout_ = UntypedLayout{};
  semantics_ = UntypedImageSemantics{};
}

template <ImageModifiability modifiability_>
std::ptrdiff_t DynImageView<modifiability_>::compute_data_offset(PixelIndex y) const noexcept
{
  return static_cast<std::ptrdiff_t>(layout_.stride_bytes * y);
}

template <ImageModifiability modifiability_>
std::ptrdiff_t DynImageView<modifiability_>::compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
{
  return static_cast<std::ptrdiff_t>(layout_.stride_bytes * y + layout_.nr_bytes_per_pixel() * x);
}

template <ImageModifiability modifiability_0, ImageModifiability modifiability_1>
bool equal(const DynImageView<modifiability_0>& dyn_img_0, const DynImageView<modifiability_1>& dyn_img_1)
{
  // Special case: if both images have a zero-length side, the shall be considered equal (both are invalid)
  if ((dyn_img_0.width() == 0 || dyn_img_0.height() == 0) && (dyn_img_1.width() == 0 || dyn_img_1.height() == 0))
  {
    return true;
  }

  if (dyn_img_0.width() != dyn_img_1.width() || dyn_img_0.height() != dyn_img_1.height())
  {
    return false;
  }

  for (auto y = 0_idx; y < dyn_img_0.height(); ++y)
  {
    const auto end0 = dyn_img_0.byte_ptr(static_cast<PixelIndex>(dyn_img_0.width()), y);
    const auto begin0 = dyn_img_0.byte_ptr(y);
    const auto begin1 = dyn_img_1.byte_ptr(y);

    // std::equal may not be optimized to std::memcmp, even though we're dealing with a POD-type here...
    // const bool equal_row = std::equal(begin0, end0, begin1);
    // ...so let's just call std::memcmp directly:
    const auto nr_bytes = static_cast<std::size_t>(std::distance(begin0, end0));
    const bool equal_row = (std::memcmp(begin0, begin1, nr_bytes) == 0);

    if (!equal_row)
    {
      return false;
    }
  }

  return true;
}

template <ImageModifiability modifiability_>
void swap(DynImageView<modifiability_>& dyn_img_view_l, DynImageView<modifiability_>& dyn_img_view_r) noexcept
{
  using std::swap;
  swap(dyn_img_view_l.ptr_, dyn_img_view_r.ptr_);
  swap(dyn_img_view_l.layout_, dyn_img_view_r.layout_);
  swap(dyn_img_view_l.semantics_, dyn_img_view_r.semantics_);
}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_DYN_IMAGE_VIEW_HPP

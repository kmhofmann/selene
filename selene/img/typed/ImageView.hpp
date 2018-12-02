// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_VIEW_HPP
#define SELENE_IMG_TYPED_IMAGE_VIEW_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>

#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageIterators.hpp>
#include <selene/img/typed/TypedLayout.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <cstring>

namespace sln {

template <typename PixelType_, ImageModifiability modifiability_>
class ImageView : public ImageBase<ImageView<PixelType_, modifiability_>>
{
public:
  using PixelType = PixelType_;
  using DataPtrType = typename DataPtr<modifiability_>::Type;
  using PixelTypePtr = std::conditional_t<modifiability_ == ImageModifiability::Constant, const PixelType*, PixelType*>;

  using iterator = ImageRowIterator<PixelType, modifiability_>;  ///< The iterator type.
  using const_iterator = ConstImageRowIterator<PixelType, modifiability_>;  ///< The const_iterator type.

//  constexpr static bool is_view = true;
//  constexpr static bool is_modifiable = (modifiability_ == ImageModifiability::Mutable);
//  constexpr static ImageModifiability modifiability() { return modifiability_; }

  constexpr static bool is_view = impl::ImageBaseTraits<ImageView<PixelType_, modifiability_>>::is_view;
  constexpr static bool is_modifiable = impl::ImageBaseTraits<ImageView<PixelType_, modifiability_>>::is_modifiable;
  constexpr static ImageModifiability modifiability() { return impl::ImageBaseTraits<ImageView<PixelType_, modifiability_>>::modifiability(); }

  ImageView() = default;
  ImageView(DataPtr<modifiability_> ptr, TypedLayout layout)
      : ptr_(ptr), layout_(layout)
  { }

  const TypedLayout& layout() const noexcept { return layout_; }

  PixelLength width() const noexcept { return layout_.width; }
  PixelLength height() const noexcept { return layout_.height; }
  Stride stride_bytes() const noexcept { return layout_.stride_bytes; }
  std::ptrdiff_t row_bytes() const noexcept { return layout_.row_bytes<PixelType>(); }
  std::ptrdiff_t total_bytes() const noexcept { return layout_.total_bytes<PixelType>(); }
  bool is_packed() const noexcept { return layout_.is_packed<PixelType>(); }

  bool is_empty() const noexcept { return ptr_.data() == nullptr || layout_.width == 0 || layout_.height == 0; }
  bool is_valid() const noexcept { return !is_empty(); };

  iterator begin() noexcept              { return ImageRowIterator<PixelType, modifiability_>(ImageRow<PixelType, modifiability_>(this, 0_idx)); }
  const_iterator begin() const noexcept  { return ConstImageRowIterator<PixelType, modifiability_>(ConstImageRow<PixelType, modifiability_>(this, 0_idx)); }
  const_iterator cbegin() const noexcept { return ConstImageRowIterator<PixelType, modifiability_>(ConstImageRow<PixelType, modifiability_>(this, 0_idx)); }

  iterator end() noexcept              { return ImageRowIterator<PixelType, modifiability_>(ImageRow<PixelType, modifiability_>(this, PixelIndex{this->height()})); }
  const_iterator end() const noexcept  { return ConstImageRowIterator<PixelType, modifiability_>(ConstImageRow<PixelType, modifiability_>(this, PixelIndex{this->height()})); }
  const_iterator cend() const noexcept { return ConstImageRowIterator<PixelType, modifiability_>(ConstImageRow<PixelType, modifiability_>(this, PixelIndex{this->height()})); }

  DataPtrType byte_ptr() noexcept             { return ptr_.data(); }
  const DataPtrType byte_ptr() const noexcept { return ptr_.data(); }

  DataPtrType byte_ptr(PixelIndex y) noexcept             { return ptr_.data() + this->compute_data_offset(y); }
  const DataPtrType byte_ptr(PixelIndex y) const noexcept { return ptr_.data() + this->compute_data_offset(y); }

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept             { return ptr_.data() + this->compute_data_offset(x, y); }
  const DataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return ptr_.data() + this->compute_data_offset(x, y); }

  PixelTypePtr data() noexcept             { return reinterpret_cast<PixelTypePtr>(this->byte_ptr()); }
  const PixelTypePtr data() const noexcept { return reinterpret_cast<const PixelTypePtr>(this->byte_ptr()); }

  PixelTypePtr data(PixelIndex y) noexcept             { return reinterpret_cast<PixelTypePtr>(this->byte_ptr(y)); }
  const PixelTypePtr data(PixelIndex y) const noexcept { return reinterpret_cast<const PixelTypePtr>(this->byte_ptr(y)); }

  PixelTypePtr data_row_end(PixelIndex y) noexcept             { return reinterpret_cast<PixelTypePtr>(this->byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * layout_.width); }
  const PixelTypePtr data_row_end(PixelIndex y) const noexcept { return reinterpret_cast<const PixelTypePtr>(this->byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * layout_.width); }

  PixelTypePtr data(PixelIndex x, PixelIndex y) noexcept             { return reinterpret_cast<PixelTypePtr>(this->byte_ptr(x, y)); }
  const PixelTypePtr data(PixelIndex x, PixelIndex y) const noexcept { return reinterpret_cast<const PixelTypePtr>(this->byte_ptr(x, y)); }

  auto& operator()(PixelIndex x, PixelIndex y) noexcept             { return *this->data(x, y); }
  const auto& operator()(PixelIndex x, PixelIndex y) const noexcept { return *this->data(x, y); }

  ImageView<PixelType, modifiability_>& view() noexcept { return *this; }
  ImageView<PixelType, ImageModifiability::Constant> view() const noexcept { return constant_view(); }  // TODO: optimize
  ImageView<PixelType, ImageModifiability::Constant> constant_view() const noexcept { return ImageView<PixelType, ImageModifiability::Constant>{this->byte_ptr(), this->layout()}; }  // TODO: optimize

  void clear()
  {
    ptr_ = DataPtr<modifiability_>{};
    layout_ = TypedLayout{};
  }

private:
  static_assert(std::is_trivial<PixelType>::value, "Pixel type is not trivial");
  static_assert(std::is_standard_layout<PixelType>::value, "Pixel type is not standard layout");

  DataPtr<modifiability_> ptr_;
  TypedLayout layout_;

  sln::Bytes compute_data_offset(PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes * y};
  }

  sln::Bytes compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes * y + PixelTraits<PixelType>::nr_bytes * x};
  }
};

template <typename PixelType> using MutableImageView = ImageView<PixelType, ImageModifiability::Mutable>;
template <typename PixelType> using ConstantImageView = ImageView<PixelType, ImageModifiability::Constant>;

template <typename PixelType0, ImageModifiability modifiability_0,
          typename PixelType1, ImageModifiability modifiability_1>
bool equal(const ImageView<PixelType0, modifiability_0>& img_0, const ImageView<PixelType1, modifiability_1>& img_1)
{
  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<PixelType0>::Element,
                    typename PixelTraits<PixelType1>::Element>::value,
                "Incompatible pixel types for equality comparison");
  static_assert(PixelTraits<PixelType0>::nr_channels == PixelTraits<PixelType1>::nr_channels,
                "Incompatible pixel types for equality comparison");
  static_assert(get_nr_channels(PixelTraits<PixelType0>::pixel_format)
                == get_nr_channels(PixelTraits<PixelType1>::pixel_format)
                || PixelTraits<PixelType0>::pixel_format == PixelFormat::Unknown
                || PixelTraits<PixelType1>::pixel_format == PixelFormat::Unknown,
                "Incompatible pixel types for equality comparison");

  // The sizes should then also be the same
  static_assert(sizeof(PixelType0) == sizeof(PixelType1), "Incompatible pixel types for equality comparison");

  // Special case: if both images have a zero-length side, the shall be considered equal (both are invalid)
  if ((img_0.width() == 0 || img_0.height() == 0) && (img_1.width() == 0 || img_1.height() == 0))
  {
    return true;
  }

  if (img_0.width() != img_1.width() || img_0.height() != img_1.height())
  {
    return false;
  }

  for (auto y = 0_idx; y < img_0.height(); ++y)
  {
    const auto end0 = img_0.data_row_end(y);
    const auto begin0 = img_0.data(y);
    const auto begin1 = img_1.data(y);

    // std::equal may not be optimized to std::memcmp, even though we're dealing with a POD-type here...
    // const bool equal_row = std::equal(begin0, end0, begin1);
    // ...so let's just call std::memcmp directly:
    const auto nr_bytes = std::distance(begin0, end0) * sizeof(PixelType0);
    const bool equal_row = (std::memcmp(begin0, begin1, nr_bytes) == 0);

    if (!equal_row)
    {
      return false;
    }
  }

  return true;
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_VIEW_HPP

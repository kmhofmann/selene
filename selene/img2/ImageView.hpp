// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_IMAGEVIEW_HPP
#define SELENE_IMG2_IMAGEVIEW_HPP

/// @file

#include <selene/img2/ImageBase.hpp>
#include <selene/img2/Layout.hpp>
#include <selene/img2/PixelTraits.hpp>

namespace sln {

template <typename PixelType_, ImageModifiability modifiability>
class ImageView : public ImageBase<ImageView<PixelType_, modifiability>>
{
public:
  using PixelType = PixelType_;
  using DataPtrType = typename DataPtr<modifiability>::Type;

  ImageView(DataPtr<modifiability> ptr, TypedLayout layout)
      : ptr_(ptr), layout_(layout)
  { }

  TypedLayout& layout() noexcept { return layout_; }
  const TypedLayout& layout() const noexcept { return layout_; }

  PixelLength width() const noexcept { return layout_.width(); }
  PixelLength height() const noexcept { return layout_.height(); }
  Stride stride_bytes() const noexcept { return layout_.stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return layout_.width() * PixelTraits<PixelType>::nr_bytes; }
  std::ptrdiff_t total_bytes() const noexcept { return layout_.stride_bytes() * layout_.height(); }
  bool is_packed() const noexcept { return layout_.stride_bytes() == static_cast<Stride::value_type>(PixelTraits<PixelType>::nr_bytes * layout_.width()); }

  bool is_empty() const noexcept { return ptr_.data() == nullptr || layout_.width() == 0 || layout_.height() == 0; }
  bool is_valid() const noexcept { return !is_empty(); };

//  iterator begin() noexcept;
//  const_iterator begin() const noexcept;
//  const_iterator cbegin() const noexcept;
//
//  iterator end() noexcept;
//  const_iterator end() const noexcept;
//  const_iterator cend() const noexcept;

  DataPtrType byte_ptr() noexcept             { return ptr_.data(); }
  const DataPtrType byte_ptr() const noexcept { return ptr_.data(); }

  DataPtrType byte_ptr(PixelIndex y) noexcept             { return ptr_.data() + this->compute_data_offset(y); }
  const DataPtrType byte_ptr(PixelIndex y) const noexcept { return ptr_.data() + this->compute_data_offset(y); }

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept             { return ptr_.data() + this->compute_data_offset(x, y); }
  const DataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return ptr_.data() + this->compute_data_offset(x, y); }

  PixelType* data() noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr()); }
  const PixelType* data() const noexcept { return reinterpret_cast<PixelType*>(this->byte_ptr()); }

  PixelType* data(PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(y)); }
  const PixelType* data(PixelIndex y) const noexcept { return reinterpret_cast<PixelType*>(this->byte_ptr(y)); }

  PixelType* data_row_end(PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * layout_.width()); }
  const PixelType* data_row_end(PixelIndex y) const noexcept { return reinterpret_cast<PixelType*>(this->byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * layout_.width()); }

  PixelType* data(PixelIndex x, PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(x, y)); }
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept { return reinterpret_cast<PixelType*>(this->byte_ptr(x, y)); }

  PixelType& operator()(PixelIndex x, PixelIndex y) noexcept             { return *this->data(x, y); }
  const PixelType& operator()(PixelIndex x, PixelIndex y) const noexcept { return *this->data(x, y); }

private:
  static_assert(std::is_trivial<PixelType>::value, "Pixel type is not trivial");
  static_assert(std::is_standard_layout<PixelType>::value, "Pixel type is not standard layout");

  DataPtr<modifiability> ptr_;
  TypedLayout layout_;

  sln::Bytes compute_data_offset(PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes() * y};
  }

  sln::Bytes compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes() * y + PixelTraits<PixelType>::nr_bytes * x};
  }
};

template <typename PixelType> using MutableImageView = ImageView<PixelType, ImageModifiability::Mutable>;
template <typename PixelType> using ConstImageView = ImageView<PixelType, ImageModifiability::Constant>;

}  // namespace sln

#endif  // SELENE_IMG2_IMAGEVIEW_HPP

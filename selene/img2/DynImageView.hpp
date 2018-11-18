// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_DYNIMAGEVIEW_HPP
#define SELENE_IMG2_DYNIMAGEVIEW_HPP

/// @file

#include <selene/img2/Layout.hpp>

#include <type_traits>

namespace sln {

template <ImageModifiability modifiability = ImageModifiability::Constant>
class DynImageView
{
public:
  using DataPtrType = typename DataPtr<modifiability>::Type;

  DynImageView(DataPtr<modifiability> ptr, UntypedLayout layout)
      : ptr_(ptr), layout_(layout)
  { }

  UntypedLayout& layout() noexcept { return layout_; }
  const UntypedLayout& layout() const noexcept { return layout_; }

  PixelLength width() const noexcept { return layout_.width(); }
  PixelLength height() const noexcept { return layout_.height(); }
  std::int16_t nr_channels() const noexcept { return layout_.nr_channels(); }
  std::int16_t nr_bytes_per_channel() const noexcept { return layout_.nr_bytes_per_channel(); }
  Stride stride_bytes() const noexcept { return layout_.stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return layout_.width() * nr_pixel_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return layout_.stride_bytes() * layout_.height(); }
  PixelFormat pixel_format() const noexcept { return semantics_.pixel_format(); }
  SampleFormat sample_format() const noexcept { return semantics_.sample_format(); }
  bool is_packed() const noexcept { return layout_.stride_bytes() == static_cast<Stride::value_type>(nr_pixel_bytes() * layout_.width()); }

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

  template <typename PixelType, typename T = void, typename = std::enable_if_t<modifiability == ImageModifiability::Mutable, T>>
  PixelType* data() noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr()); }

  template <typename PixelType>
  const PixelType* data() const noexcept { return reinterpret_cast<const PixelType*>(this->byte_ptr()); }

  template <typename PixelType, typename T = void, typename = std::enable_if_t<modifiability == ImageModifiability::Mutable, T>>
  PixelType* data(PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(y)); }

  template <typename PixelType>
  const PixelType* data(PixelIndex y) const noexcept { return reinterpret_cast<const PixelType*>(this->byte_ptr(y)); }

  template <typename PixelType, typename T = void, typename = std::enable_if_t<modifiability == ImageModifiability::Mutable, T>>
  PixelType* data_row_end(PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(y) + nr_pixel_bytes() * layout_.width()); }

  template <typename PixelType>
  const PixelType* data_row_end(PixelIndex y) const noexcept { return reinterpret_cast<const PixelType*>(this->byte_ptr(y) + nr_pixel_bytes() * layout_.width()); }

  template <typename PixelType, typename T = void, typename = std::enable_if_t<modifiability == ImageModifiability::Mutable, T>>
  PixelType* data(PixelIndex x, PixelIndex y) noexcept             { return reinterpret_cast<PixelType*>(this->byte_ptr(x, y)); }

  template <typename PixelType>
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept { return reinterpret_cast<const PixelType*>(this->byte_ptr(x, y)); }

  template <typename PixelType, typename T = void, typename = std::enable_if_t<modifiability == ImageModifiability::Mutable, T>>
  PixelType& pixel(PixelIndex x, PixelIndex y) noexcept             { return *data<PixelType>(x, y); }

  template <typename PixelType>
  const PixelType& pixel(PixelIndex x, PixelIndex y) const noexcept { return *data<PixelType>(x, y); }

private:
  DataPtr<modifiability> ptr_;
  UntypedLayout layout_;
  UntypedImageSemantics semantics_;

  std::int16_t nr_pixel_bytes() const noexcept
  {
    return layout_.nr_bytes_per_channel() * layout_.nr_channels();
  }

  sln::Bytes compute_data_offset(PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes() * y};
  }

  sln::Bytes compute_data_offset(PixelIndex x, PixelIndex y) const noexcept
  {
    return sln::Bytes{layout_.stride_bytes() * y + nr_pixel_bytes() * x};
  }
};

}  // namespace sln

#endif  // SELENE_IMG2_DYNIMAGEVIEW_HPP

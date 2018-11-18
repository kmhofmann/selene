// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_IMAGE_BASE_HPP
#define SELENE_IMG2_IMAGE_BASE_HPP

/// @file

#include <selene/img2/Layout.hpp>

namespace sln {

template <typename Derived>
class ImageBase
{
public:
  ImageBase() = default;
  ~ImageBase() = default;

  ImageBase(const ImageBase<Derived>&) = delete;
  ImageBase<Derived>& operator=(const ImageBase<Derived>&) = delete;

  ImageBase(ImageBase<Derived>&&) noexcept = delete;
  ImageBase<Derived>& operator=(ImageBase<Derived>&&) noexcept = delete;

  TypedLayout& layout() noexcept { return static_cast<Derived*>(this)->layout(); }
  const TypedLayout& layout() const noexcept { return static_cast<Derived*>(this)->layout(); }

  PixelLength width() const noexcept { return static_cast<const Derived*>(this)->width(); }
  PixelLength height() const noexcept { return static_cast<const Derived*>(this)->height(); }
  Stride stride_bytes() const noexcept { return static_cast<const Derived*>(this)->stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return static_cast<const Derived*>(this)->row_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return static_cast<const Derived*>(this)->total_bytes(); }
  bool is_packed() const noexcept { return static_cast<const Derived*>(this)->is_packed(); }

  bool is_empty() const noexcept { return static_cast<const Derived*>(this)->is_empty(); }
  bool is_valid() const noexcept { return static_cast<const Derived*>(this)->is_valid(); }

//  auto begin() noexcept { return static_cast<Derived*>(this)->begin(); }
//  auto begin() const noexcept { return static_cast<Derived*>(this)->begin(); }
//  auto cbegin() const noexcept { return static_cast<Derived*>(this)->cbegin(); }
//
//  auto end() noexcept { return static_cast<Derived*>(this)->end(); }
//  auto end() const noexcept  { return static_cast<Derived*>(this)->end(); }
//  auto cend() const noexcept { return static_cast<Derived*>(this)->cend(); }

  auto byte_ptr() noexcept       { return static_cast<Derived*>(this)->byte_ptr(); }
  auto byte_ptr() const noexcept { return static_cast<const Derived*>(this)->byte_ptr(); }

  auto byte_ptr(PixelIndex y) noexcept       { return static_cast<Derived*>(this)->byte_ptr(y); }
  auto byte_ptr(PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->byte_ptr(y); }

  auto byte_ptr(PixelIndex x, PixelIndex y) noexcept       { return static_cast<Derived*>(this)->byte_ptr(x, y); }
  auto byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->byte_ptr(x, y); }

  auto data() noexcept       { return static_cast<Derived*>(this)->data(); }
  auto data() const noexcept { return static_cast<const Derived*>(this)->data(); }

  auto data(PixelIndex y) noexcept       { return static_cast<Derived*>(this)->data(y); }
  auto data(PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->data(y); }

  auto data_row_end(PixelIndex y) noexcept       { return static_cast<Derived*>(this)->data_row_end(y); }
  auto data_row_end(PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->data_row_end(y); }

  auto data(PixelIndex x, PixelIndex y) noexcept       { return static_cast<Derived*>(this)->data(x, y); }
  auto data(PixelIndex x, PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->data(x, y); }

  auto operator()(PixelIndex x, PixelIndex y) noexcept       { return static_cast<Derived*>(this)->operator()(x, y); }
  auto operator()(PixelIndex x, PixelIndex y) const noexcept { return static_cast<const Derived*>(this)->operator()(x, y); }
};

}  // namespace sln

#endif  // SELENE_IMG2_IMAGE_BASE_HPP

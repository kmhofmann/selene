// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_BASE_HPP
#define SELENE_IMG_IMAGE_BASE_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>
#include <selene/img/typed/TypedLayout.hpp>

namespace sln {

template <typename Derived>
struct ImageBaseTraits;

template <typename Derived>
class ImageBase
{
public:
  using PixelType = typename ImageBaseTraits<Derived>::PixelType;

  constexpr static bool is_view = Derived::is_view;
  constexpr static bool is_modifiable = Derived::is_modifiable;
  constexpr static ImageModifiability modifiability() { return Derived::modifiability(); }

  Derived& derived() noexcept { return *static_cast<Derived*>(this); }
  const Derived& derived() const noexcept { return *static_cast<const Derived*>(this); }

  const TypedLayout& layout() const noexcept { return derived().layout(); }

  PixelLength width() const noexcept { return derived().width(); }
  PixelLength height() const noexcept { return derived().height(); }
  Stride stride_bytes() const noexcept { return derived().stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return derived().row_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return derived().total_bytes(); }
  bool is_packed() const noexcept { return derived().is_packed(); }

  bool is_empty() const noexcept { return derived().is_empty(); }
  bool is_valid() const noexcept { return derived().is_valid(); }

  auto begin() noexcept { return derived().begin(); }
  auto begin() const noexcept { return derived().begin(); }
  auto cbegin() const noexcept { return derived().cbegin(); }

  auto end() noexcept { return derived().end(); }
  auto end() const noexcept  { return derived().end(); }
  auto cend() const noexcept { return derived().cend(); }

  auto byte_ptr() noexcept       { return derived().byte_ptr(); }
  auto byte_ptr() const noexcept { return derived().byte_ptr(); }

  auto byte_ptr(PixelIndex y) noexcept       { return derived().byte_ptr(y); }
  auto byte_ptr(PixelIndex y) const noexcept { return derived().byte_ptr(y); }

  auto byte_ptr(PixelIndex x, PixelIndex y) noexcept       { return derived().byte_ptr(x, y); }
  auto byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return derived().byte_ptr(x, y); }

  auto data() noexcept       { return derived().data(); }
  auto data() const noexcept { return derived().data(); }

  auto data(PixelIndex y) noexcept       { return derived().data(y); }
  auto data(PixelIndex y) const noexcept { return derived().data(y); }

  auto data_row_end(PixelIndex y) noexcept       { return derived().data_row_end(y); }
  auto data_row_end(PixelIndex y) const noexcept { return derived().data_row_end(y); }

  auto data(PixelIndex x, PixelIndex y) noexcept       { return derived().data(x, y); }
  auto data(PixelIndex x, PixelIndex y) const noexcept { return derived().data(x, y); }

  auto operator()(PixelIndex x, PixelIndex y) noexcept       { return derived().operator()(x, y); }
  auto operator()(PixelIndex x, PixelIndex y) const noexcept { return derived().operator()(x, y); }

  auto view() noexcept       { return derived().view(); }
  auto view() const noexcept { return derived().view(); }

  auto clear() { return derived().clear(); }
};

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_BASE_HPP

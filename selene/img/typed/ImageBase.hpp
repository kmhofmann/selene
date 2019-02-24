// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_BASE_HPP
#define SELENE_IMG_TYPED_IMAGE_BASE_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>

#include <selene/img/typed/TypedLayout.hpp>
#include <selene/img/typed/_impl/ImageExprTraits.hpp>
#include <selene/img/typed/_impl/StaticChecks.hpp>

#include <type_traits>

namespace sln {

/// \addtogroup group-img-typed
/// @{

template <typename Derived> class ImageExpr;

template <typename Derived>
class ImageExpr
{
public:
//  static_assert(impl::is_image_type_v<Derived>);

  using PixelType = typename impl::ImageExprTraits<Derived>::PixelType;

  constexpr static bool is_view = impl::ImageExprTraits<Derived>::is_view;
  constexpr static bool is_modifiable = impl::ImageExprTraits<Derived>::is_modifiable;

  constexpr static ImageModifiability modifiability() { return is_modifiable ? ImageModifiability::Mutable : ImageModifiability::Constant; }

  Derived& derived() noexcept { return *static_cast<Derived*>(this); }
  const Derived& derived() const noexcept { return *static_cast<const Derived*>(this); }

  decltype(auto) layout() const noexcept { return this->derived().layout(); }

  PixelLength width() const noexcept { return derived().width(); }
  PixelLength height() const noexcept { return derived().height(); }
  Stride stride_bytes() const noexcept { return this->derived().stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) noexcept       { return derived().operator()(x, y); }
  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept { return derived().operator()(x, y); }
};


template <typename Derived>
class ImageBase : public ImageExpr<Derived>
{
public:
  using PixelType = typename ImageExpr<Derived>::PixelType;

  std::ptrdiff_t row_bytes() const noexcept { return this->derived().row_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return this->derived().total_bytes(); }
  bool is_packed() const noexcept { return this->derived().is_packed(); }

  bool is_empty() const noexcept { return this->derived().is_empty(); }
  bool is_valid() const noexcept { return this->derived().is_valid(); }

  decltype(auto) begin() noexcept { return this->derived().begin(); }
  decltype(auto) begin() const noexcept { return this->derived().begin(); }
  decltype(auto) cbegin() const noexcept { return this->derived().cbegin(); }

  decltype(auto) end() noexcept { return this->derived().end(); }
  decltype(auto) end() const noexcept  { return this->derived().end(); }
  decltype(auto) cend() const noexcept { return this->derived().cend(); }

  decltype(auto) byte_ptr() noexcept       { return this->derived().byte_ptr(); }
  decltype(auto) byte_ptr() const noexcept { return this->derived().byte_ptr(); }

  decltype(auto) byte_ptr(PixelIndex y) noexcept       { return this->derived().byte_ptr(y); }
  decltype(auto) byte_ptr(PixelIndex y) const noexcept { return this->derived().byte_ptr(y); }

  decltype(auto) byte_ptr(PixelIndex x, PixelIndex y) noexcept       { return this->derived().byte_ptr(x, y); }
  decltype(auto) byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return this->derived().byte_ptr(x, y); }

  decltype(auto) data() noexcept       { return this->derived().data(); }
  decltype(auto) data() const noexcept { return this->derived().data(); }

  decltype(auto) data(PixelIndex y) noexcept       { return this->derived().data(y); }
  decltype(auto) data(PixelIndex y) const noexcept { return this->derived().data(y); }

  decltype(auto) data_row_end(PixelIndex y) noexcept       { return this->derived().data_row_end(y); }
  decltype(auto) data_row_end(PixelIndex y) const noexcept { return this->derived().data_row_end(y); }

  decltype(auto) data(PixelIndex x, PixelIndex y) noexcept       { return this->derived().data(x, y); }
  decltype(auto) data(PixelIndex x, PixelIndex y) const noexcept { return this->derived().data(x, y); }

  decltype(auto) view() noexcept       { return this->derived().view(); }
  decltype(auto) view() const noexcept { return this->derived().view(); }
  decltype(auto) constant_view() const noexcept { return this->derived().constant_view(); }

  decltype(auto) clear() { return this->derived().clear(); }
};

/// @}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_BASE_HPP

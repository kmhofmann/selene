// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_BASE_HPP
#define SELENE_IMG_TYPED_IMAGE_BASE_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>

#include <selene/img/typed/ImageBaseTraits.hpp>
#include <selene/img/typed/TypedLayout.hpp>

namespace sln {

template <typename Derived>
class ImageBase
{
public:
  using PixelType = typename ImageBaseTraits<Derived>::PixelType;

  constexpr static bool is_view = ImageBaseTraits<Derived>::is_view;
  constexpr static bool is_modifiable = ImageBaseTraits<Derived>::is_modifiable;
  constexpr static ImageModifiability modifiability() { return ImageBaseTraits<Derived>::modifiability(); }

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

  decltype(auto) begin() noexcept { return derived().begin(); }
  decltype(auto) begin() const noexcept { return derived().begin(); }
  decltype(auto) cbegin() const noexcept { return derived().cbegin(); }

  decltype(auto) end() noexcept { return derived().end(); }
  decltype(auto) end() const noexcept  { return derived().end(); }
  decltype(auto) cend() const noexcept { return derived().cend(); }

  decltype(auto) byte_ptr() noexcept       { return derived().byte_ptr(); }
  decltype(auto) byte_ptr() const noexcept { return derived().byte_ptr(); }

  decltype(auto) byte_ptr(PixelIndex y) noexcept       { return derived().byte_ptr(y); }
  decltype(auto) byte_ptr(PixelIndex y) const noexcept { return derived().byte_ptr(y); }

  decltype(auto) byte_ptr(PixelIndex x, PixelIndex y) noexcept       { return derived().byte_ptr(x, y); }
  decltype(auto) byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return derived().byte_ptr(x, y); }

  decltype(auto) data() noexcept       { return derived().data(); }
  decltype(auto) data() const noexcept { return derived().data(); }

  decltype(auto) data(PixelIndex y) noexcept       { return derived().data(y); }
  decltype(auto) data(PixelIndex y) const noexcept { return derived().data(y); }

  decltype(auto) data_row_end(PixelIndex y) noexcept       { return derived().data_row_end(y); }
  decltype(auto) data_row_end(PixelIndex y) const noexcept { return derived().data_row_end(y); }

  decltype(auto) data(PixelIndex x, PixelIndex y) noexcept       { return derived().data(x, y); }
  decltype(auto) data(PixelIndex x, PixelIndex y) const noexcept { return derived().data(x, y); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) noexcept       { return derived().operator()(x, y); }
  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept { return derived().operator()(x, y); }

  decltype(auto) view() noexcept       { return derived().view(); }
  decltype(auto) view() const noexcept { return derived().view(); }
  decltype(auto) constant_view() const noexcept { return derived().constant_view(); }

  decltype(auto) clear() { return derived().clear(); }
};

// -----

template <typename ImageType, typename = void>
struct IsImageType : std::false_type
{ };

template <typename ImageType>
struct IsImageType<ImageType, std::void_t<typename ImageType::PixelType,
                              decltype(std::declval<ImageType>().is_view),
                              decltype(std::declval<ImageType>().is_modifiable)>> : std::true_type
{ };

template <typename ImageType>
inline constexpr bool is_image_type_v = IsImageType<ImageType>::value;

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_BASE_HPP

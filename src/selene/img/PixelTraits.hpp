// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_TRAITS_HPP
#define SELENE_IMG_PIXEL_TRAITS_HPP

#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace selene {
namespace img {

template <typename Element_>
struct PixelTraits
{
  using Element = Element_;
  static constexpr std::uint16_t nr_channels = 1;
  static constexpr std::uint8_t nr_bytes = sizeof(Element);
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element);

  static constexpr bool is_integral = std::is_integral<Element>::value;
  static constexpr bool is_floating_point = std::is_floating_point<Element>::value;
  static constexpr bool is_unsigned = std::is_unsigned<Element>::value;

  static constexpr SampleType sample_type =
      std::is_integral<Element>::value ? (std::is_unsigned<Element>::value ? SampleType::UnsignedInteger
                                                                           : SampleType::SignedInteger)
                                       : (std::is_floating_point<Element>::value ? SampleType::FloatingPoint
                                                                                 : SampleType::Unknown);
};

template <typename T, std::uint32_t N>
struct PixelTraits<Pixel<T, N>>
{
  using Element = Pixel<T, N>;
  static constexpr std::uint16_t nr_channels = N;
  static constexpr std::uint8_t nr_bytes = sizeof(Element);
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element) / N;

  static constexpr bool is_integral = std::is_integral<T>::value;
  static constexpr bool is_floating_point = std::is_floating_point<T>::value;
  static constexpr bool is_unsigned = std::is_unsigned<T>::value;

  static constexpr SampleType sample_type =
      std::is_integral<T>::value ? (std::is_unsigned<T>::value ? SampleType::UnsignedInteger
                                                               : SampleType::SignedInteger)
                                 : (std::is_floating_point<T>::value ? SampleType::FloatingPoint
                                                                     : SampleType::Unknown);
};

} // namespace img
} // namespace selene

#endif // SELENE_IMG_PIXEL_TRAITS_HPP
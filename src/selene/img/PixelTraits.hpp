// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_TRAITS_HPP
#define SELENE_IMG_PIXEL_TRAITS_HPP

/// @file

#include <selene/base/Utils.hpp>

#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace sln {

/** \brief Class representing traits of a pixel.
 *
 * @tparam Element_ The pixel element type.
 */
template <typename Element_>
struct PixelTraits
{
  using Element = Element_;  ///< The pixel element type.
  static constexpr std::uint32_t nr_channels = 1;  ///< The number of channels per pixel.
  static constexpr std::uint8_t nr_bytes = sizeof(Element);  ///< The number of bytes per pixel.
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element);  ///< The number of bytes per pixel channel.

  /// True, if the pixel elements are integral values; false otherwise.
  static constexpr bool is_integral = std::is_integral<Element>::value;

  /// True, if the pixel elements are floating point values; false otherwise.
  static constexpr bool is_floating_point = std::is_floating_point<Element>::value;

  /// True, if the pixel elements are unsigned; false otherwise.
  static constexpr bool is_unsigned = std::is_unsigned<Element>::value;

  /// The sample format (unsigned/signed integer or floating point number).
  static constexpr SampleFormat sample_format =
      std::is_integral<Element>::value
          ? (std::is_unsigned<Element>::value ? SampleFormat::UnsignedInteger : SampleFormat::SignedInteger)
          : (std::is_floating_point<Element>::value ? SampleFormat::FloatingPoint : SampleFormat::Unknown);

  /// The value of the zero element.
  static constexpr Element zero_element = Element{0};
};

/** \brief Class representing traits of a pixel. Specialization for `Pixel<T, N>`.
 *
 * @tparam T The channel element type of the pixel type.
 * @tparam N The number of channels of the pixel type.
 */
template <typename T, std::uint32_t N>
struct PixelTraits<Pixel<T, N>>
{
  using Element = T;  ///< The pixel element type.
  static constexpr std::uint32_t nr_channels = N;  ///< The number of channels per pixel.
  static constexpr std::uint8_t nr_bytes = sizeof(Pixel<T, N>);  ///< The number of bytes per pixel.
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element);  ///< The number of bytes per pixel channel.

  /// True, if the pixel elements are integral values; false otherwise.
  static constexpr bool is_integral = std::is_integral<T>::value;

  /// True, if the pixel elements are floating point values; false otherwise.
  static constexpr bool is_floating_point = std::is_floating_point<T>::value;

  /// True, if the pixel elements are unsigned; false otherwise.
  static constexpr bool is_unsigned = std::is_unsigned<T>::value;

  /// The sample format (unsigned/signed integer or floating point number).
  static constexpr SampleFormat sample_format = std::is_integral<T>::value
                                                    ? (std::is_unsigned<T>::value ? SampleFormat::UnsignedInteger
                                                                                  : SampleFormat::SignedInteger)
                                                    : (std::is_floating_point<T>::value ? SampleFormat::FloatingPoint
                                                                                        : SampleFormat::Unknown);

  /// The value of the zero element.
  static constexpr Pixel<T, N> zero_element = Pixel<T, N>{make_array_n_equal<Element, N>(0)};
};

// Out-of-line definitions for non-integral static declarations above:

template <typename Element_>
constexpr Element_ PixelTraits<Element_>::zero_element;

template <typename T, std::uint32_t N>
constexpr Pixel<T, N> PixelTraits<Pixel<T, N>>::zero_element;

}  // namespace sln

#endif  // SELENE_IMG_PIXEL_TRAITS_HPP

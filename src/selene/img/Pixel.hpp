// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_HPP
#define SELENE_IMG_PIXEL_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/Types.hpp>

#include <array>
#include <cstdint>
#include <type_traits>

namespace sln {

/** \brief Represents a statically typed pixel with arbitrary number of channels.
 *
 * A Pixel<T, n> represents an image element with static type T and n number of samples (channels) of this type per
 * element.
 * A template instance of Pixel<T, n> is guaranteed to be a tightly packed POD type.
 *
 * \tparam T The type for each channel element.
 * \tparam nr_channels_ The number of channels.
 */
template <typename T, std::uint32_t nr_channels_>
class Pixel
{
public:
  using value_type = T;  ///< The type of each pixel sample (channel).
  static constexpr auto nr_channels = nr_channels_;  ///< The number of channels per pixel.

  Pixel() = default;  ///< Default constructor. Pixel values are uninitialized.

  template <typename... Args, typename = typename std::enable_if_t<sizeof...(Args) == nr_channels_>>
  constexpr explicit Pixel(Args... args);

  constexpr explicit Pixel(const std::array<T, nr_channels>& arr);
  ~Pixel() = default;

  Pixel(const Pixel<T, nr_channels_>&) = default;  ///< Copy constructor.
  Pixel<T, nr_channels_>& operator=(const Pixel<T, nr_channels_>&) = default;  ///< Copy assignment operator.
  Pixel(Pixel<T, nr_channels_>&&) noexcept = default;  ///< Move constructor.
  Pixel<T, nr_channels_>& operator=(Pixel<T, nr_channels_>&&) noexcept = default;  ///< Move assignment operator.

  T* data() noexcept;
  const T* data() const noexcept;

  constexpr T& operator[](std::size_t n) noexcept;
  constexpr const T& operator[](std::size_t n) const noexcept;

  // Allow implicit conversion to T, iff nr_channels_ ==1:
  operator std::conditional_t<nr_channels_ == 1, T, void>() const;

private:
  static_assert(std::is_arithmetic<T>::value, "Pixel type needs to be an arithmetic type");
  std::array<T, nr_channels> data_;
};

template <typename T, std::uint32_t nr_channels_>
bool operator==(const Pixel<T, nr_channels_>& px0, const Pixel<T, nr_channels_>& px1);

template <typename T, std::uint32_t nr_channels_>
bool operator!=(const Pixel<T, nr_channels_>& px0, const Pixel<T, nr_channels_>& px1);


// ----------
// Aliases:

using Pixel_8u1 = Pixel<std::uint8_t, 1>;  ///< 8-bit unsigned 1-channel pixel.
using Pixel_8u2 = Pixel<std::uint8_t, 2>;  ///< 8-bit unsigned 2-channel pixel.
using Pixel_8u3 = Pixel<std::uint8_t, 3>;  ///< 8-bit unsigned 3-channel pixel.
using Pixel_8u4 = Pixel<std::uint8_t, 4>;  ///< 8-bit unsigned 4-channel pixel.

using Pixel_8s1 = Pixel<std::int8_t, 1>;  ///< 8-bit signed 1-channel pixel.
using Pixel_8s2 = Pixel<std::int8_t, 2>;  ///< 8-bit signed 2-channel pixel.
using Pixel_8s3 = Pixel<std::int8_t, 3>;  ///< 8-bit signed 3-channel pixel.
using Pixel_8s4 = Pixel<std::int8_t, 4>;  ///< 8-bit signed 4-channel pixel.

using Pixel_16u1 = Pixel<std::uint16_t, 1>;  ///< 16-bit unsigned 1-channel pixel.
using Pixel_16u2 = Pixel<std::uint16_t, 2>;  ///< 16-bit unsigned 2-channel pixel.
using Pixel_16u3 = Pixel<std::uint16_t, 3>;  ///< 16-bit unsigned 3-channel pixel.
using Pixel_16u4 = Pixel<std::uint16_t, 4>;  ///< 16-bit unsigned 4-channel pixel.

using Pixel_16s1 = Pixel<std::int16_t, 1>;  ///< 16-bit signed 1-channel pixel.
using Pixel_16s2 = Pixel<std::int16_t, 2>;  ///< 16-bit signed 2-channel pixel.
using Pixel_16s3 = Pixel<std::int16_t, 3>;  ///< 16-bit signed 3-channel pixel.
using Pixel_16s4 = Pixel<std::int16_t, 4>;  ///< 16-bit signed 4-channel pixel.

using Pixel_32u1 = Pixel<std::uint32_t, 1>;  ///< 32-bit unsigned 1-channel pixel.
using Pixel_32u2 = Pixel<std::uint32_t, 2>;  ///< 32-bit unsigned 2-channel pixel.
using Pixel_32u3 = Pixel<std::uint32_t, 3>;  ///< 32-bit unsigned 3-channel pixel.
using Pixel_32u4 = Pixel<std::uint32_t, 4>;  ///< 32-bit unsigned 4-channel pixel.

using Pixel_32s1 = Pixel<std::int32_t, 1>;  ///< 32-bit signed 1-channel pixel.
using Pixel_32s2 = Pixel<std::int32_t, 2>;  ///< 32-bit signed 2-channel pixel.
using Pixel_32s3 = Pixel<std::int32_t, 3>;  ///< 32-bit signed 3-channel pixel.
using Pixel_32s4 = Pixel<std::int32_t, 4>;  ///< 32-bit signed 4-channel pixel.

using Pixel_64u1 = Pixel<std::uint64_t, 1>;  ///< 64-bit unsigned 1-channel pixel.
using Pixel_64u2 = Pixel<std::uint64_t, 2>;  ///< 64-bit unsigned 2-channel pixel.
using Pixel_64u3 = Pixel<std::uint64_t, 3>;  ///< 64-bit unsigned 3-channel pixel.
using Pixel_64u4 = Pixel<std::uint64_t, 4>;  ///< 64-bit unsigned 4-channel pixel.

using Pixel_64s1 = Pixel<std::int64_t, 1>;  ///< 64-bit signed 1-channel pixel.
using Pixel_64s2 = Pixel<std::int64_t, 2>;  ///< 64-bit signed 2-channel pixel.
using Pixel_64s3 = Pixel<std::int64_t, 3>;  ///< 64-bit signed 3-channel pixel.
using Pixel_64s4 = Pixel<std::int64_t, 4>;  ///< 64-bit signed 4-channel pixel.

using Pixel_32f1 = Pixel<float32_t, 1>;  ///< 32-bit floating point 1-channel pixel.
using Pixel_32f2 = Pixel<float32_t, 2>;  ///< 32-bit floating point 2-channel pixel.
using Pixel_32f3 = Pixel<float32_t, 3>;  ///< 32-bit floating point 3-channel pixel.
using Pixel_32f4 = Pixel<float32_t, 4>;  ///< 32-bit floating point 4-channel pixel.

using Pixel_64f1 = Pixel<float64_t, 1>;  ///< 64-bit floating point 1-channel pixel.
using Pixel_64f2 = Pixel<float64_t, 2>;  ///< 64-bit floating point 2-channel pixel.
using Pixel_64f3 = Pixel<float64_t, 3>;  ///< 64-bit floating point 3-channel pixel.
using Pixel_64f4 = Pixel<float64_t, 4>;  ///< 64-bit floating point 4-channel pixel.

// ----------
// Implementation:

/** \brief Constructor initializing all pixel channels.
 *
 * This constructor takes exactly Pixel::nr_channels channel values of Pixel::value_type as separate argument inputs.
 *
 * \param args The channel values as separate arguments.
 */
template <typename T, std::uint32_t nr_channels_>
template <typename... Args, typename>
inline constexpr Pixel<T, nr_channels_>::Pixel(Args... args) : data_{{static_cast<T>(args)...}}
{
  static_assert(std::is_pod<Pixel<T, nr_channels_>>::value, "Pixel type is not POD");
  static_assert(sizeof(Pixel<T, nr_channels_>) == nr_channels_ * sizeof(T), "Pixel class is not tightly packed");
}

/** \brief Constructor initializing all pixel channels.
 *
 * This constructor takes a std::array<> with exactly Pixel::nr_channels values of type Pixel::value_type as input.
 *
 * \param arr The channel values as std::array<>.
 */
template <typename T, std::uint32_t nr_channels_>
inline constexpr Pixel<T, nr_channels_>::Pixel(const std::array<T, nr_channels>& arr) : data_(arr)
{
}

/** \brief Returns a pointer to the first element of the pixel.
 *
 * \return Pointer to the first pixel element.
 */
template <typename T, std::uint32_t nr_channels_>
inline T* Pixel<T, nr_channels_>::data() noexcept
{
  return data_.data();
}

/** \brief Returns a constant pointer to the first element of the pixel.
 *
 * \return Constant pointer to the first pixel element.
 */
template <typename T, std::uint32_t nr_channels_>
inline const T* Pixel<T, nr_channels_>::data() const noexcept
{
  return data_.data();
}

/** \brief Provides read-write access to the n-th channel element of the pixel.
 *
 * \param n Index of the channel to access.
 * \return Reference to the n-th channel element.
 */
template <typename T, std::uint32_t nr_channels_>
inline constexpr T& Pixel<T, nr_channels_>::operator[](std::size_t n) noexcept
{
  SELENE_ASSERT(n < nr_channels);
  return data_[n];
}

/** \brief Provides read access to the n-th channel element of the pixel.
 *
 * \param n Index of the channel to access.
 * \return Const reference to the n-th channel element.
 */
template <typename T, std::uint32_t nr_channels_>
inline constexpr const T& Pixel<T, nr_channels_>::operator[](std::size_t n) const noexcept
{
  SELENE_ASSERT(n < nr_channels);
  return data_[n];
}

/** \brief Allows implicit conversion to Pixel::value_type, iff Pixel::nr_channels == 1.
 *
 * This is useful for usage with any single-channel pixel instance, because the 0-th element will not have to be
 * accessed by operator[] anymore, but the instance decays to type Pixel::value_type.
 */
template <typename T, std::uint32_t nr_channels_>
inline Pixel<T, nr_channels_>::operator std::conditional_t<nr_channels_ == 1, T, void>() const
{
  return data_[0];
}

// ----------

/** \brief Equality comparison for two pixels.
 *
 * \param px0 The left-hand side pixel to compare.
 * \param px1 The right-hand side pixel to compare.
 * \return True, if the two pixels have equal values for all channels; false otherwise.
 */
template <typename T, std::uint32_t nr_channels_>
inline bool operator==(const Pixel<T, nr_channels_>& px0, const Pixel<T, nr_channels_>& px1)
{
  bool equal = true;

  for (auto i = 0u; i < nr_channels_; ++i)
  {
    equal &= (px0[i] == px1[i]);
  }

  return equal;
}

/** \brief Inequality comparison for two pixels.
 *
 * \param px0 The left-hand side pixel to compare.
 * \param px1 The right-hand side pixel to compare.
 * \return True, if the two pixels are not equal; false, if they are equal.
 */
template <typename T, std::uint32_t nr_channels_>
inline bool operator!=(const Pixel<T, nr_channels_>& px0, const Pixel<T, nr_channels_>& px1)
{
  return !(px0 == px1);
}

}  // namespace sln

#endif  // SELENE_IMG_PIXEL_HPP

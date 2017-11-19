// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_HPP
#define SELENE_IMG_PIXEL_HPP

#include <selene/base/Types.hpp>

#include <array>
#include <cstdint>
#include <type_traits>

namespace selene {
namespace img {

template <typename T, std::uint32_t nr_channels_> class Pixel
{
public:
  static constexpr auto nr_channels = nr_channels_;

  Pixel() = default;

  template <typename... Args, typename = typename std::enable_if_t<sizeof...(Args) == nr_channels_>>
    constexpr explicit Pixel(Args... args);

  constexpr explicit Pixel(const std::array<T, nr_channels>& arr);
  ~Pixel() = default;

  Pixel(const Pixel<T, nr_channels_>&) = default;
  Pixel<T, nr_channels_>& operator=(const Pixel<T, nr_channels_>&) = default;
  Pixel(Pixel<T, nr_channels_>&&) noexcept = default;
  Pixel<T, nr_channels_>& operator=(Pixel<T, nr_channels_>&&) noexcept = default;

  T* data();
  const T* data() const;

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

using Pixel_8u1 = Pixel<std::uint8_t, 1>;
using Pixel_8u2 = Pixel<std::uint8_t, 2>;
using Pixel_8u3 = Pixel<std::uint8_t, 3>;
using Pixel_8u4 = Pixel<std::uint8_t, 4>;

using Pixel_8s1 = Pixel<std::int8_t, 1>;
using Pixel_8s2 = Pixel<std::int8_t, 2>;
using Pixel_8s3 = Pixel<std::int8_t, 3>;
using Pixel_8s4 = Pixel<std::int8_t, 4>;

using Pixel_16u1 = Pixel<std::uint16_t, 1>;
using Pixel_16u2 = Pixel<std::uint16_t, 2>;
using Pixel_16u3 = Pixel<std::uint16_t, 3>;
using Pixel_16u4 = Pixel<std::uint16_t, 4>;

using Pixel_16s1 = Pixel<std::int16_t, 1>;
using Pixel_16s2 = Pixel<std::int16_t, 2>;
using Pixel_16s3 = Pixel<std::int16_t, 3>;
using Pixel_16s4 = Pixel<std::int16_t, 4>;

using Pixel_32u1 = Pixel<std::uint32_t, 1>;
using Pixel_32u2 = Pixel<std::uint32_t, 2>;
using Pixel_32u3 = Pixel<std::uint32_t, 3>;
using Pixel_32u4 = Pixel<std::uint32_t, 4>;

using Pixel_32s1 = Pixel<std::int32_t, 1>;
using Pixel_32s2 = Pixel<std::int32_t, 2>;
using Pixel_32s3 = Pixel<std::int32_t, 3>;
using Pixel_32s4 = Pixel<std::int32_t, 4>;

using Pixel_64u1 = Pixel<std::uint64_t, 1>;
using Pixel_64u2 = Pixel<std::uint64_t, 2>;
using Pixel_64u3 = Pixel<std::uint64_t, 3>;
using Pixel_64u4 = Pixel<std::uint64_t, 4>;

using Pixel_64s1 = Pixel<std::int64_t, 1>;
using Pixel_64s2 = Pixel<std::int64_t, 2>;
using Pixel_64s3 = Pixel<std::int64_t, 3>;
using Pixel_64s4 = Pixel<std::int64_t, 4>;

using Pixel_32f1 = Pixel<float32_t, 1>;
using Pixel_32f2 = Pixel<float32_t, 2>;
using Pixel_32f3 = Pixel<float32_t, 3>;
using Pixel_32f4 = Pixel<float32_t, 4>;

using Pixel_64f1 = Pixel<float64_t, 1>;
using Pixel_64f2 = Pixel<float64_t, 2>;
using Pixel_64f3 = Pixel<float64_t, 3>;
using Pixel_64f4 = Pixel<float64_t, 4>;

// ----------
// Implementation:

template <typename T, std::uint32_t nr_channels_>
template <typename... Args, typename>
inline constexpr Pixel<T, nr_channels_>::Pixel(Args... args)
    : data_{{static_cast<T>(args)...}}
{
  static_assert(std::is_pod<Pixel<T, nr_channels_>>::value, "Pixel type is not POD");
  static_assert(sizeof(Pixel<T, nr_channels_>) == nr_channels_ * sizeof(T), "Pixel class is not tightly packed");
}

template <typename T, std::uint32_t nr_channels_>
inline constexpr Pixel<T, nr_channels_>::Pixel(const std::array<T, nr_channels>& arr)
    : data_(arr)
{
}

template <typename T, std::uint32_t nr_channels_>
inline T* Pixel<T, nr_channels_>::data()
{
  return data_.data();
}

template <typename T, std::uint32_t nr_channels_>
inline const T* Pixel<T, nr_channels_>::data() const
{
  return data_.data();
}

template <typename T, std::uint32_t nr_channels_>
inline constexpr T& Pixel<T, nr_channels_>::operator[](std::size_t n) noexcept
{
  return data_[n];
}

template <typename T, std::uint32_t nr_channels_>
inline constexpr const T& Pixel<T, nr_channels_>::operator[](std::size_t n) const noexcept
{
  return data_[n];
}

template <typename T, std::uint32_t nr_channels_>
inline Pixel<T, nr_channels_>::operator std::conditional_t<nr_channels_ == 1, T, void>() const
{
  return data_[0];
}

// ----------

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

template <typename T, std::uint32_t nr_channels_>
inline bool operator!=(const Pixel<T, nr_channels_>& px0, const Pixel<T, nr_channels_>& px1)
{
  return !(px0 == px1);
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_PIXEL_HPP
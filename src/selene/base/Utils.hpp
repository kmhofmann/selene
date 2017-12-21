// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_UTILS_HPP
#define SELENE_BASE_UTILS_HPP

/// @file

#include <selene/base/Assert.hpp>

#include <array>
#include <cstdint>
#include <functional>

namespace sln {

/** \brief Clamp a value between a minimum and a maximum value.
 *
 * If `value` compares less than `min`, returns `min`. Otherwise if `max` compares less than `value`, returns `max`.
 * Otherwise returns `value`.
 *
 * @tparam T The value type.
 * @tparam Compare The comparison function.
 * @param value The value to clamp.
 * @param min The minimum value/lower bound.
 * @param max The maximum value/upper bound.
 * @param comp The comparison function.
 * @return The clamped value.
 */
template <typename T, typename Compare>
inline constexpr const T& clamp(const T& value, const T& min, const T& max, Compare comp)
{
  SELENE_ASSERT(!comp(max, min));
  return comp(value, min) ? min : comp(max, value) ? max : value;
}

/** \brief Clamp a value between a minimum and a maximum value.
 *
 * If `value` compares less than `min`, returns `min`. Otherwise if `max` compares less than `value`, returns `max`.
 * Otherwise returns `value`. Uses `operator<()` to compare the values.
 *
 * Given usual semantics, the function returns max(a, min(b, x)), where x is the input value and a, b are minimum and
 * maximum, respectively.
 *
 * @tparam T The value type.
 * @param value The value to clamp.
 * @param min The minimum value/lower bound.
 * @param max The maximum value/upper bound.
 * @return The clamped value.
 */
template <typename T>
inline constexpr const T& clamp(const T& value, const T& min, const T& max)
{
  return clamp(value, min, max, std::less<T>());
}

/** \brief Returns a `std::array` filled with `N` equal values.
 *
 * @tparam T The value type of the array.
 * @tparam N The number of elements in the array.
 * @param value The value to fill the array with.
 * @return A `std::array<T, N>`, where each entry has value `value`.
 */
template <typename T, std::uint32_t N>
constexpr std::array<T, N> make_array_n_equal(T value)
{
  using result_t = ::std::array<T, N>;
  result_t arr = {{value}};
  const result_t& const_arr = arr;

  for (std::uint32_t i = 1; i < N; ++i)
  {
    // const_cast not needed anymore in C++17: https://stackoverflow.com/a/46779579/218634
    const_cast<typename result_t::reference>(const_arr[i]) = value;
  }

  return arr;
}

}  // namespace sln

#endif  // SELENE_BASE_UTILS_HPP

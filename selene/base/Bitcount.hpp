// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_BITCOUNT_HPP
#define SELENE_BASE_BITCOUNT_HPP

/// @file

#include <climits>
#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#endif  // defined(_MSC_VER)

namespace sln {

/** \brief Performs a bit count on the supplied value.
 *
 * @tparam T The value type.
 * @param x The input value.
 * @return The number of set bits.
 */
template <typename T>
constexpr std::size_t bit_count(T x)
{
  // from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
  x = static_cast<T>(x - ((x >> 1) & static_cast<T>(~T{0}) / 3));
  x = static_cast<T>((x & static_cast<T>(~T{0}) / 15 * 3) + ((x >> 2) & static_cast<T>(~T{0}) / 15 * 3));
  x = static_cast<T>((x + (x >> 4)) & static_cast<T>(~T{0}) / 255 * 15);
  const std::size_t c = static_cast<T>(x * ((T)~T{0} / 255)) >> (sizeof(T) - 1) * CHAR_BIT;
  return c;
}

#if ((defined (__GNUG__) || defined(__GNUC__)) && defined(__SSE4_2__) && (__SIZEOF_INT__ == 4) && (__SIZEOF_LONG__ == 8))

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned int x)
{
  return __builtin_popcount(x);
}

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned long x)
{
  return __builtin_popcountl(x);
}

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned long long x)
{
  return __builtin_popcountll(x);
}

#elif defined(_MSC_VER)

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned short x)
{
  return __popcnt16(x);
}

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned int x)
{
  return __popcnt(x);
}

#if _WIN64

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
inline std::size_t bit_count(unsigned __int64 x)
{
  return __popcnt64(x);
}

#endif  // _WIN64

#else

/** \brief Performs a bit count on the supplied value.
 *
 * @param x The input value.
 * @return The number of set bits.
 */
constexpr std::size_t bit_count(std::uint32_t x)
{
  // from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  const std::size_t c = (((x + (x >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
  return c;
}

#endif

}  // namespace sln

#endif  // SELENE_BASE_BITCOUNT_HPP

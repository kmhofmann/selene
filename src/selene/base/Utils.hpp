// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_UTILS_HPP
#define SELENE_BASE_UTILS_HPP

/// @file

#include <cstdint>

namespace selene {

/** \brief Clamp a value between a minimum and a maximum value.
 *
 * Mathematically, the function returns max(a, min(b, x)), where x is the input value and a, b are minimum and maximum,
 * respectively.
 *
 * \param value The value to clamp.
 * \param min The minimum value/lower bound.
 * \param max The maximum value/upper bound.
 * \return The clamped value.
 */
template <typename T>
inline T clamp(T value, T min, T max)
{
  return (value < min) ? min : ((value > max) ? max : value);
}

}  // namespace selene

#endif  // SELENE_BASE_UTILS_HPP

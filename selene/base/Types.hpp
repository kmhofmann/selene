// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_TYPES_HPP
#define SELENE_BASE_TYPES_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/_impl/ExplicitType.hpp>

namespace sln {

using float32_t = float;  ///< 32-bit floating point type.
using float64_t = double;  ///< 64-bit floating point type.

// This can be set via a CMake option inside the library.
#if defined(SELENE_DEFAULT_SCALAR_SINGLE_PRECISION)
using default_float_t = float32_t;  ///< Default floating point type: single precision.
#else
using default_float_t = float64_t;  ///< Default floating point type: double precision.
#endif

static_assert(sizeof(float32_t) == 4, "type size mismatch");
static_assert(sizeof(float64_t) == 8, "type size mismatch");

namespace impl {

class BytesTag;

}  // namespace impl

using Bytes = sln::impl::ExplicitType<std::ptrdiff_t, sln::impl::BytesTag>;  ///< Type representing a number of bytes.

inline namespace literals {

/** \brief User-defined literal representing a number of bytes.
 *
 * @param nr_bytes Number of bytes.
 * @return A `Bytes` instance.
 */
constexpr inline Bytes operator"" _b(unsigned long long nr_bytes) noexcept
{
  return Bytes(static_cast<Bytes::value_type>(nr_bytes));
}

}  // namespace literals

}  // namespace sln

#endif  // SELENE_BASE_TYPES_HPP

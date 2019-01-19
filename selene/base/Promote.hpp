// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_PROMOTE_HPP
#define SELENE_BASE_PROMOTE_HPP

/// @file

#include <selene/base/Types.hpp>

#include <cstdint>

namespace sln {

/// \addtogroup group-base
/// @{

/** \brief Promote an integral type to its next larger integral type.
 *
 * Contains a using-declaration of `type` representing the promoted type.
 *
 * Explicit specializations for single and double precision floating point types are provided, which return identity.
 *
 * @tparam T The type to be promoted.
 */
template <typename T>
struct promote;

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::uint8_t>
{
  using type = std::uint16_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::int8_t>
{
  using type = std::int16_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::uint16_t>
{
  using type = std::uint32_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::int16_t>
{
  using type = std::int32_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::uint32_t>
{
  using type = std::uint64_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::int32_t>
{
  using type = std::int64_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::uint64_t>
{
  using type = std::uint64_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<std::int64_t>
{
  using type = std::int64_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<float32_t>
{
  using type = float32_t;  ///< The promoted type.
};

/** \brief Promote an integral type to its next larger integral type. Explicit specialization. */
template <>
struct promote<float64_t>
{
  using type = float64_t;  ///< The promoted type.
};

template <typename T>
using promote_t = typename promote<T>::type;  ///< Helper type for `promote<>`.

/// @}

}  // namespace sln

#endif  // SELENE_BASE_PROMOTE_HPP

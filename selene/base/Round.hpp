// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_ROUND_HPP
#define SELENE_BASE_ROUND_HPP

/// @file

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace sln {

template <typename Result, typename Value>
constexpr Result constexpr_floor(Value val) noexcept;

template <typename Result, typename Value>
constexpr Result constexpr_ceil(Value val) noexcept;

template <typename Result, typename Value>
Result round_half_up(Value val) noexcept;

template <typename Result, typename Value>
constexpr Result constexpr_round_half_up(Value val) noexcept;

template <typename Result, typename Value>
Result round_half_down(Value val) noexcept;

template <typename Result, typename Value>
constexpr Result constexpr_round_half_down(Value val) noexcept;

template <typename Result, typename Value>
Result round(Value val) noexcept;

template <typename Result, typename Value>
constexpr Result constexpr_round(Value val) noexcept;


// ----------
// Implementation:

/** \brief Floor operation, i.e. computes the largest integer value not greater than `val`.
 *
 * This function can (and should only!) be used in a guaranteed constexpr context.
 * Otherwise use the equivalent `std::floor`, which is likely to be more efficient.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The output value after applying the "floor" operation.
 */
template <typename Result, typename Value>
constexpr Result constexpr_floor(Value val) noexcept
{
  const auto ival = static_cast<Result>(val);

  if (val == static_cast<Value>(ival))
  {
    return ival;
  }

  return static_cast<Result>(ival - (val < 0 ? 1 : 0));
}

/** \brief Ceil operation, i.e. computes the smalles integer value not less than `val`.
 *
 * This function can (and should only!) be used in a guaranteed constexpr context.
 * Otherwise use the equivalent `std::ceil`, which is likely to be more efficient.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The output value after applying the "ceil" operation.
 */
template <typename Result, typename Value>
constexpr Result constexpr_ceil(Value val) noexcept
{
  const auto ival = static_cast<Result>(val);

  if (val == static_cast<Value>(ival))
  {
    return ival;
  }

  return static_cast<Result>(ival + (val > 0 ? 1 : 0));
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding is biased, such that values with fraction 0.5 will always be rounded up.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
inline Result round_half_up(Value val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  static_assert(std::is_floating_point<Value>::value, "Argument to round not floating point");
  return static_cast<Result>(std::floor(val + Value{0.5}));
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding is biased, such that values with fraction 0.5 will always be rounded up.
 *
 * This function can (and should only!) be used in a guaranteed constexpr context.
 * Otherwise use the equivalent `sln::round_half_up`, which is likely to be more efficient.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
constexpr Result constexpr_round_half_up(Value val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  static_assert(std::is_floating_point<Value>::value, "Argument to round not floating point");
  return constexpr_floor<Result>(val + Value{0.5});
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding is biased, such that values with fraction 0.5 will always be rounded down.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
inline Result round_half_down(Value val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  static_assert(std::is_floating_point<Value>::value, "Argument to round not floating point");
  return static_cast<Result>(std::ceil(val - Value{0.5}));
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding is biased, such that values with fraction 0.5 will always be rounded down.
 *
 * This function can (and should only!) be used in a guaranteed constexpr context.
 * Otherwise use the equivalent `sln::round_half_down`, which is likely to be more efficient.
 *
 * @tparam Result The result value type. Has to be an integral type.
 * @tparam Value The input value type. Has to be a floating point type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
constexpr Result constexpr_round_half_down(Value val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  static_assert(std::is_floating_point<Value>::value, "Argument to round not floating point");
  return constexpr_ceil<Result>(val - Value{0.5});
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding may be biased in an unspecified way when rounding values with fraction 0.5.
 *
 * @tparam Result The result value type.
 * @tparam Value The input value type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
inline Result round(Value val) noexcept
{
  // TODO: Would calling std::lrint() be faster than introducing a conditional branch? (It would change behavior...)
  return (val >= 0) ? round_half_up<Result>(val) : round_half_down<Result>(val);
}

/** \brief Rounds the given floating point value to the nearest integer value.
 *
 * The rounding may be biased in an unspecified way when rounding values with fraction 0.5.
 *
 * This function can (and should only!) be used in a guaranteed constexpr context.
 * Otherwise use the equivalent `sln::round`, which is likely to be more efficient.
 *
 * @tparam Result The result value type.
 * @tparam Value The input value type.
 * @param val The input value.
 * @return The rounded output value.
 */
template <typename Result, typename Value>
constexpr Result constexpr_round(Value val) noexcept
{
  return (val >= 0) ? constexpr_round_half_up<Result>(val) : constexpr_round_half_down<Result>(val);
}


/// \cond INTERNAL

template <typename Result>
constexpr Result round(char val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(signed char val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(unsigned char val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(short val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(unsigned short val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(int val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(unsigned int val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(long val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(unsigned long val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(long long val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

template <typename Result>
constexpr Result round(unsigned long long val) noexcept
{
  static_assert(std::is_integral<Result>::value, "Cannot round to non-integral type");
  return static_cast<Result>(val);
}

/// \endcond

}  // namespace sln

#endif  // SELENE_BASE_ROUND_HPP

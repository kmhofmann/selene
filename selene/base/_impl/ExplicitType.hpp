// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_STRONG_TYPE_HPP
#define SELENE_BASE_STRONG_TYPE_HPP

/// @file

#include <type_traits>
#include <utility>

namespace sln {
namespace impl {

/** \brief Utility class, representing a value type as an explicit, "strong" typedef.
 *
 * Intended for internal library use.
 *
 * Note that conversions from an instantiation of ExplicitType<> to the underlying value type are implicit.
 *
 * @tparam ValueType_ The value type.
 * @tparam TagType_ A tag type for type disambiguation. Not used inside the class.
 */
template <typename ValueType_, typename TagType_>
class ExplicitType
{
  /// \cond INTERNAL
public:
  using value_type = ValueType_;
  using tag_type = TagType_;

  constexpr ExplicitType() noexcept : value_()
  {
  }

  constexpr explicit ExplicitType(const value_type& value) noexcept : value_(value)
  {
  }

  constexpr explicit ExplicitType(value_type&& value) noexcept(std::is_nothrow_move_constructible<value_type>::value)
      : value_(value)
  {
  }

  constexpr /*explicit*/ operator value_type&() noexcept
  {
    return value_;
  }

  constexpr /*explicit*/ operator const value_type&() const noexcept
  {
    return value_;
  }

  constexpr ExplicitType<ValueType_, TagType_> operator-() noexcept
  {
    return ExplicitType<ValueType_, TagType_>(-value_);
  }

  friend constexpr void swap(ExplicitType& a, ExplicitType& b) noexcept
  {
    using std::swap;
    swap(static_cast<value_type&>(a), static_cast<value_type&>(b));
  }

private:
  static_assert(std::is_arithmetic_v<value_type>);

  value_type value_;
  /// \endcond
};

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_STRONG_TYPE_HPP

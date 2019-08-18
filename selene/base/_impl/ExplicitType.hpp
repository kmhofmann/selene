// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IMPL_EXPLICIT_TYPE_HPP
#define SELENE_BASE_IMPL_EXPLICIT_TYPE_HPP

#include <ostream>
#include <type_traits>
#include <utility>

namespace sln {
namespace impl {

/** \brief Utility class, representing a value type as an explicit, "strong" typedef.
 *
 * Intended for internal library use.
 *
 * Note that conversions from an instance of ExplicitType<> to another type have to be explicit.
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

  constexpr ExplicitType() = default;

  constexpr ExplicitType(const ExplicitType<ValueType_, TagType_>&) noexcept = default;
  constexpr ExplicitType& operator=(const ExplicitType<ValueType_, TagType_>&) noexcept = default;

  constexpr ExplicitType(ExplicitType<ValueType_, TagType_>&&) noexcept = default;
  constexpr ExplicitType& operator=(ExplicitType<ValueType_, TagType_>&&) noexcept = default;

  constexpr explicit ExplicitType(const value_type& value) noexcept : value_(value)
  {
  }

  constexpr explicit ExplicitType(value_type&& value) noexcept(std::is_nothrow_move_constructible_v<value_type>)
      : value_(value)
  {
  }

  constexpr explicit operator value_type&() noexcept
  {
    return value_;
  }

  constexpr explicit operator const value_type&() const noexcept
  {
    return value_;
  }

  template <typename T, typename = std::enable_if_t<!std::is_same_v<T, value_type>>>
  constexpr explicit operator T() noexcept
  {
    return static_cast<T>(value_);
  }

  template <typename T, typename = std::enable_if_t<!std::is_same_v<T, value_type>>>
  constexpr explicit operator const T() const noexcept
  {
    return static_cast<T>(value_);
  }

  constexpr ExplicitType<ValueType_, TagType_> operator-() noexcept
  {
    return ExplicitType<ValueType_, TagType_>(-value_);
  }

  constexpr ExplicitType<ValueType_, TagType_>& operator++()
  {
    ++value_;
    return *this;
  }

  const ExplicitType<ValueType_, TagType_> operator++(int)
  {
    ExplicitType<ValueType_, TagType_> tmp(*this);
    this->operator++();
    return tmp;
  }

  constexpr ExplicitType<ValueType_, TagType_>& operator--()
  {
    --value_;
    return *this;
  }

  const ExplicitType<ValueType_, TagType_> operator--(int)
  {
    ExplicitType<ValueType_, TagType_> tmp(*this);
    this->operator--();
    return tmp;
  }

  friend constexpr void swap(ExplicitType& a, ExplicitType& b) noexcept
  {
    using std::swap;
    swap(static_cast<value_type&>(a), static_cast<value_type&>(b));
  }

  constexpr value_type& value()
  {
    return value_;
  }

  constexpr value_type value() const
  {
    return value_;
  }

private:
  static_assert(std::is_arithmetic_v<value_type>);

  value_type value_;
  /// \endcond
};

template <typename Value>
auto value(Value value)
{
  return value;
}

template <typename ValueType, typename TagType>
auto value(ExplicitType<ValueType, TagType> value)
{
  return value.value();
}

template <typename ValueType_, typename TagType_>
std::ostream& operator<<(std::ostream& os, const ExplicitType<ValueType_, TagType_>& value)
{
  os << value.value();
  return os;
}

// ----------

template <typename... Ts> struct ET_UniqueTypeType;

template <typename... Ts>
struct ET_UniqueType
{
  using type = ET_UniqueTypeType<Ts...>;
};

template <typename... Ts> using ET_UniqueType_t = typename ET_UniqueType<Ts...>::type;

// Helper macros

#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_SAME(OP)                \
  template <typename ValueType_, typename TagType_>                \
  constexpr auto operator OP(ExplicitType<ValueType_, TagType_> l, \
                             ExplicitType<ValueType_, TagType_> r) \
  {                                                                \
    return l.value() OP r.value();                                 \
  }                                                                \


#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_DIFFERENT(OP)           \
template <typename ValueTypeL_, typename TagTypeL_,                \
          typename ValueTypeR_, typename TagTypeR_>                \
constexpr auto operator OP(ExplicitType<ValueTypeL_, TagTypeL_> l, \
                           ExplicitType<ValueTypeR_, TagTypeR_> r) \
{                                                                  \
  return l.value() OP r.value();                                   \
}                                                                  \

#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_LEFT(OP)                \
  template <typename ValueType_, typename TagType_, typename T,    \
            typename = std::enable_if_t<std::is_arithmetic_v<T>>>  \
  constexpr auto operator OP(ExplicitType<ValueType_, TagType_> l, \
                             T r)                                  \
  {                                                                \
    return l.value() OP r;                                         \
  }                                                                \

#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_RIGHT(OP)               \
  template <typename ValueType_, typename TagType_, typename T,    \
            typename = std::enable_if_t<std::is_arithmetic_v<T>>>  \
  constexpr auto operator OP(T l,                                  \
                             ExplicitType<ValueType_, TagType_> r) \
  {                                                                \
    return l OP r.value();                                         \
  }                                                                \

#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_MIXED(OP) \
  SELENE_EXPLICIT_TYPE_COMPARISON_OP_SAME(OP)        \
  SELENE_EXPLICIT_TYPE_COMPARISON_OP_LEFT(OP)        \
  SELENE_EXPLICIT_TYPE_COMPARISON_OP_RIGHT(OP)       \

#define SELENE_EXPLICIT_TYPE_COMPARISON_OP_ANY(OP) \
  SELENE_EXPLICIT_TYPE_COMPARISON_OP_MIXED(OP)     \
  SELENE_EXPLICIT_TYPE_COMPARISON_OP_DIFFERENT(OP) \

SELENE_EXPLICIT_TYPE_COMPARISON_OP_MIXED(==)
SELENE_EXPLICIT_TYPE_COMPARISON_OP_MIXED(!=)
SELENE_EXPLICIT_TYPE_COMPARISON_OP_ANY(<)
SELENE_EXPLICIT_TYPE_COMPARISON_OP_ANY(<=)
SELENE_EXPLICIT_TYPE_COMPARISON_OP_ANY(>)
SELENE_EXPLICIT_TYPE_COMPARISON_OP_ANY(>=)

#undef SELENE_EXPLICIT_TYPE_COMPARISON_OP_SAME
#undef SELENE_EXPLICIT_TYPE_COMPARISON_OP_DIFFERENT
#undef SELENE_EXPLICIT_TYPE_COMPARISON_OP_LEFT
#undef SELENE_EXPLICIT_TYPE_COMPARISON_OP_RIGHT
#undef SELENE_EXPLICIT_TYPE_COMPARISON_OP_MIXED

// ---

#define SELENE_EXPLICIT_TYPE_BINARY_OP_SAME(OP)                              \
  template <typename ValueType_, typename TagType_>                          \
  constexpr decltype(auto) operator OP(ExplicitType<ValueType_, TagType_> l, \
                             ExplicitType<ValueType_, TagType_> r)           \
  {                                                                          \
    return ExplicitType<ValueType_, TagType_>(l.value() OP r.value());       \
  }                                                                          \

#define SELENE_EXPLICIT_TYPE_BINARY_OP_DIFFERENT(OP)                         \
template <typename ValueTypeL_, typename TagTypeL_,                          \
          typename ValueTypeR_, typename TagTypeR_>                          \
constexpr decltype(auto) operator OP(ExplicitType<ValueTypeL_, TagTypeL_> l, \
                           ExplicitType<ValueTypeR_, TagTypeR_> r)           \
{                                                                            \
  using ValueType = std::common_type_t<ValueTypeL_, ValueTypeR_>;            \
  using TagType = ET_UniqueType_t<TagTypeL_, TagTypeR_>;                     \
  return ExplicitType<ValueType, TagType>(                                   \
             static_cast<ValueType>(l.value() OP r.value()));                \
}                                                                            \

#define SELENE_EXPLICIT_TYPE_BINARY_OP_LEFT(OP)                              \
  template <typename ValueType_, typename TagType_, typename T,              \
            typename = std::enable_if_t<std::is_arithmetic_v<T>>>            \
  constexpr decltype(auto) operator OP(ExplicitType<ValueType_, TagType_> l, \
                                       T r)                                  \
  {                                                                          \
    using ValueType = std::common_type_t<ValueType_, T>;                     \
    using TagType = TagType_;                                                \
    return ExplicitType<ValueType, TagType>(                                 \
               static_cast<ValueType>(l.value() OP r));                      \
  }                                                                          \

#define SELENE_EXPLICIT_TYPE_BINARY_OP_RIGHT(OP)                             \
  template <typename ValueType_, typename TagType_, typename T,              \
            typename = std::enable_if_t<std::is_arithmetic_v<T>>>            \
  constexpr decltype(auto) operator OP(T l,                                  \
                                       ExplicitType<ValueType_, TagType_> r) \
  {                                                                          \
    using ValueType = std::common_type_t<T, ValueType_>;                     \
    using TagType = TagType_;                                                \
    return ExplicitType<ValueType, TagType>(                                 \
               static_cast<ValueType>(l OP r.value()));                      \
  }                                                                          \
                                                                             \
  template <typename ValueType_, typename TagType_, typename T>              \
  constexpr decltype(auto) operator OP(T* l,                                 \
                                       ExplicitType<ValueType_, TagType_> r) \
  {                                                                          \
    return l OP r.value();                                                   \
  }                                                                          \

#define SELENE_EXPLICIT_TYPE_BINARY_OP_MIXED(OP) \
  SELENE_EXPLICIT_TYPE_BINARY_OP_SAME(OP)       \
  SELENE_EXPLICIT_TYPE_BINARY_OP_LEFT(OP)       \
  SELENE_EXPLICIT_TYPE_BINARY_OP_RIGHT(OP)      \

#define SELENE_EXPLICIT_TYPE_BINARY_OP_ANY(OP) \
  SELENE_EXPLICIT_TYPE_BINARY_OP_MIXED(OP)     \
  SELENE_EXPLICIT_TYPE_BINARY_OP_DIFFERENT(OP) \

SELENE_EXPLICIT_TYPE_BINARY_OP_MIXED(+)
SELENE_EXPLICIT_TYPE_BINARY_OP_DIFFERENT(+)

SELENE_EXPLICIT_TYPE_BINARY_OP_MIXED(-)
SELENE_EXPLICIT_TYPE_BINARY_OP_DIFFERENT(-)

SELENE_EXPLICIT_TYPE_BINARY_OP_ANY(*)

SELENE_EXPLICIT_TYPE_BINARY_OP_SAME(/)
SELENE_EXPLICIT_TYPE_BINARY_OP_LEFT(/)  // ?

SELENE_EXPLICIT_TYPE_BINARY_OP_ANY(%)

#undef SELENE_EXPLICIT_TYPE_BINARY_OP_SAME
#undef SELENE_EXPLICIT_TYPE_BINARY_OP_DIFFERENT
#undef SELENE_EXPLICIT_TYPE_BINARY_OP_LEFT
#undef SELENE_EXPLICIT_TYPE_BINARY_OP_RIGHT
#undef SELENE_EXPLICIT_TYPE_BINARY_OP_MIXED

// ---

#define SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME(OP)                                                  \
  template <typename ValueType_, typename TagType_>                                                \
  constexpr ExplicitType<ValueType_, TagType_>& operator OP(ExplicitType<ValueType_, TagType_>& l, \
                                                            ExplicitType<ValueType_, TagType_> r)  \
  {                                                                                                \
    l.value() OP r.value();                                                                        \
    return l;                                                                                      \
  }                                                                                                \

SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME(+=)
SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME(-=)
SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME(*=)
SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME(/=)

#undef SELENE_EXPLICIT_TYPE_COMPOUND_OP_SAME

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IMPL_EXPLICIT_TYPE_HPP

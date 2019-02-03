// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IMPL_COMPRESSED_PAIR_HPP
#define SELENE_BASE_IMPL_COMPRESSED_PAIR_HPP

#include <cstddef>
#include <utility>

#include <type_traits>

// A rather minimal implementation of a compressed pair type.
// For a nice explanation, see http://talesofcpp.fusionfenix.com/post-18/episode-ten-when-size-does-matter.
// For a more robust implementation, see for example https://github.com/Morwenn/tight_pair.

namespace sln {
namespace impl {

template <typename T, std::size_t idx, bool = std::is_empty_v<T> && !std::is_final_v<T>>
class CompressedMember : private T
{
public:
  constexpr CompressedMember() noexcept(std::is_nothrow_default_constructible<T>::value)
  { }

  template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
  constexpr explicit CompressedMember(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
      : T(std::forward<U>(other))
  { }

  CompressedMember(const CompressedMember&) = default;
  CompressedMember& operator=(const CompressedMember&) = default;
  CompressedMember(CompressedMember&&) noexcept = default;
  CompressedMember& operator=(CompressedMember&&) noexcept = default;

  constexpr T& get() noexcept { return *this; }
  constexpr const T& get() const noexcept { return *this; }
};

template <typename T, std::size_t idx>
class CompressedMember<T, idx, false>
{
public:
  constexpr CompressedMember() noexcept(std::is_nothrow_default_constructible<T>::value)
      : value_()
  { }

  template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
  constexpr explicit CompressedMember(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
      : value_(std::forward<U>(other))
  { }

  template <typename U>
  constexpr auto operator=(U&& other) noexcept(std::is_nothrow_assignable<T&, U>::value) -> CompressedMember&
  {
    value_ = std::forward<U>(other);
    return *this;
  }

  CompressedMember(const CompressedMember&) = default;
  CompressedMember& operator=(const CompressedMember&) = default;
  CompressedMember(CompressedMember&&) noexcept = default;
  CompressedMember& operator=(CompressedMember&&) noexcept = default;

  constexpr T& get() noexcept { return value_; }
  constexpr const T& get() const noexcept { return value_; }

private:
  T value_;
};

template <typename T, typename U>
class CompressedPair
    : private impl::CompressedMember<T, 0>
    , private impl::CompressedMember<U, 1>
{
public:
  constexpr CompressedPair() noexcept(std::is_nothrow_default_constructible_v<T>
                                      && std::is_nothrow_default_constructible_v<U>)
      : CompressedMember<T, 0>(), CompressedMember<U, 1>()
  {
  }

  template <typename T2 = T, typename U2 = U>
  constexpr CompressedPair(T2&& first, U2&& second) noexcept(std::is_nothrow_default_constructible_v<T2>
                                                             && std::is_nothrow_default_constructible_v<U2>)
      : CompressedMember<T, 0>(std::forward<T2>(first))
      , CompressedMember<U, 1>(std::forward<U2>(second))
  {
  }

  CompressedPair(const CompressedPair&) = default;
  CompressedPair& operator=(const CompressedPair&) = default;
  CompressedPair(CompressedPair&&) noexcept = default;
  CompressedPair& operator=(CompressedPair&&) noexcept = default;

  constexpr T& first() noexcept { return CompressedMember<T, 0>::get(); }
  constexpr const T& first() const noexcept { return CompressedMember<T, 0>::get(); }

  constexpr U& second() noexcept { return CompressedMember<U, 1>::get(); }
  constexpr const U& second() const noexcept { return CompressedMember<U, 1>::get(); }

  constexpr void swap(CompressedPair& other) noexcept(std::is_nothrow_swappable_v<T>&& std::is_nothrow_swappable_v<U>)
  {
    std::swap(this->first(), other.first());
    std::swap(this->second(), other.second());
  }
};

template <typename T, typename U,
          typename = std::enable_if_t<std::is_swappable_v<T> && std::is_swappable_v<U>>>
constexpr void swap(CompressedPair<T, U>& l, CompressedPair<T, U>& r) noexcept(noexcept(l.swap(r)))
{
  l.swap(r);
}

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IMPL_COMPRESSED_PAIR_HPP
// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IO_IMPL_MEMORY_FUNCTIONS_HPP
#define SELENE_BASE_IO_IMPL_MEMORY_FUNCTIONS_HPP

#include <selene/base/Assert.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace sln {
namespace impl {

template <typename T>
inline bool memory_read_value(const std::uint8_t* data, std::ptrdiff_t len, const std::uint8_t*& ptr,
                                     T& value) noexcept
{
  SELENE_ASSERT(ptr != nullptr);

  if (ptr + sizeof(T) > data + len)
  {
    return false;
  }

  std::memcpy(&value, ptr, sizeof(T));  // memory access might be unaligned
  ptr += sizeof(T);
  return true;
}

template <typename T>
inline std::size_t memory_read_values(const std::uint8_t* data, std::ptrdiff_t len, const std::uint8_t*& ptr,
                                             T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(ptr != nullptr);
  const auto data_ = std::intptr_t(data);
  const auto len_ = std::intptr_t(len);
  const auto ptr_ = std::intptr_t(ptr);
  const auto nr_values_available = static_cast<std::ptrdiff_t>((data_ + len_- ptr_) / ptrdiff_t(sizeof(T)));
  auto nr_values_read = std::min(std::max(std::ptrdiff_t(0), nr_values_available),
                                 static_cast<std::ptrdiff_t>(nr_values));
  std::memcpy(values, ptr, static_cast<std::size_t>(nr_values_read) * sizeof(T));
  ptr += static_cast<std::size_t>(nr_values_read) * sizeof(T);
  return static_cast<std::size_t>(nr_values_read);
}

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IO_IMPL_MEMORY_FUNCTIONS_HPP

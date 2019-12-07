// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IO_IMPL_VECTOR_FUNCTIONS_HPP
#define SELENE_BASE_IO_IMPL_VECTOR_FUNCTIONS_HPP

#include <selene/base/Assert.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace sln {
namespace impl {

template <typename T>
inline bool vector_read_value(const std::vector<std::uint8_t>* data, std::ptrdiff_t& pos, T& value) noexcept
{
  SELENE_ASSERT(data);
  SELENE_ASSERT(pos <= static_cast<std::ptrdiff_t>(data->size()));

  const std::size_t len = sizeof(T);
  const auto bytes_to_end = data->size() - static_cast<std::size_t>(pos);

  if (bytes_to_end < len)
  {
    return false;
  }

  std::memcpy(&value, data->data() + pos, len);
  pos += len;
  return true;
}

template <typename T>
inline std::size_t vector_read_values(std::vector<std::uint8_t>* data, std::ptrdiff_t& pos,
                                      T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(data);
  SELENE_ASSERT(pos <= static_cast<std::ptrdiff_t>(data->size()));

  const std::size_t len = sizeof(T);
  const auto bytes_to_end = data->size() - static_cast<std::size_t>(pos);
  const auto available_values = std::min(bytes_to_end / len, nr_values);

  std::memcpy(values, data->data() + pos, available_values * len);
  pos += available_values * len;
  return available_values;
}

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IO_IMPL_VECTOR_FUNCTIONS_HPP

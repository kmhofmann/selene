// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IO_IMPL_FILE_FUNCTIONS_HPP
#define SELENE_BASE_IO_IMPL_FILE_FUNCTIONS_HPP

#include <selene/base/Assert.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace sln {
namespace impl {

template <typename T>
inline bool file_read_value(FILE* fp, T& value) noexcept
{
  SELENE_ASSERT(fp);
  const auto nr_values_read = std::fread(&value, sizeof(T), 1, fp);
  return (nr_values_read == 1);
}

template <typename T>
inline std::size_t file_read_values(FILE* fp, T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(fp);
  const auto nr_values_read = std::fread(values, sizeof(T), nr_values, fp);
  return nr_values_read;
}

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IO_IMPL_FILE_FUNCTIONS_HPP

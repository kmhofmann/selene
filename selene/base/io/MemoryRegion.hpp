// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_MEMORY_REGION_HPP
#define SELENE_IO_MEMORY_REGION_HPP

/// @file

#include <cstdint>
#include <cstdlib>

namespace sln {

struct ConstantMemoryRegion
{
  const std::uint8_t* data{nullptr};
  std::size_t len{0};
};

struct MutableMemoryRegion
{
  std::uint8_t* data{nullptr};
  std::size_t len{0};
};

}  // namespace sln

#endif  // SELENE_IO_MEMORY_REGION_HPP

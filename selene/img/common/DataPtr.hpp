// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_COMMON_DATA_PTR_HPP
#define SELENE_IMG_COMMON_DATA_PTR_HPP

/// @file

#include <cstdint>

namespace sln {

enum class ImageModifiability
{
  Constant,
  Mutable
};

// -----------

template <ImageModifiability>
class DataPtr;

// ---

template <>
class DataPtr<ImageModifiability::Constant>
{
public:
  using Type = const std::uint8_t*;
  using ConstType = const std::uint8_t*;

  DataPtr() noexcept
      : data_(nullptr)
  { }

  /*explicit*/ DataPtr(const std::uint8_t* data) noexcept
      : data_(data)
  { }

  const std::uint8_t* data() const noexcept { return data_; }

private:
  const std::uint8_t* data_;
};

// ---

template <>
class DataPtr<ImageModifiability::Mutable>
{
public:
  using Type = std::uint8_t*;
  using ConstType = const std::uint8_t*;

  DataPtr() noexcept
      : data_(nullptr)
  { }

  /*explicit*/ DataPtr(std::uint8_t* data) noexcept
      : data_(data)
  { }

  std::uint8_t* data() noexcept { return data_; }
  std::uint8_t* data() const noexcept { return data_; }

private:
  std::uint8_t* data_;
};

}  // namespace sln

#endif  // SELENE_IMG_COMMON_DATA_PTR_HPP

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace selene {
namespace io {

class MemoryReader
{
public:
  MemoryReader() = default;
  MemoryReader(const std::uint8_t* data, std::size_t len);
  ~MemoryReader() = default;

  MemoryReader(const MemoryReader&) = delete;
  MemoryReader& operator=(const MemoryReader&) = delete;
  MemoryReader(MemoryReader&&) noexcept = default;
  MemoryReader& operator=(MemoryReader&&) noexcept = default;

  const std::uint8_t* handle() noexcept;

  bool open(const std::uint8_t* data, std::size_t len) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;
  std::size_t size() const noexcept;
  std::ptrdiff_t bytes_remaining() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  bool read(T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  std::size_t read(T* values, std::size_t nr_values) noexcept;

private:
  const std::uint8_t* data_ = nullptr;
  std::ptrdiff_t len_ = 0;
  mutable const std::uint8_t* ptr_ = nullptr;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
T read(MemoryReader& source);

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool read(MemoryReader& source, T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t read(MemoryReader& source, T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline MemoryReader::MemoryReader(const std::uint8_t* data, std::size_t len)
{
  if (!open(data, len))
  {
    throw std::runtime_error("Invalid memory region");
  }
}

inline const std::uint8_t* MemoryReader::handle() noexcept
{
  return ptr_;
}

inline bool MemoryReader::open(const std::uint8_t* data, std::size_t len) noexcept
{
  if (data == nullptr || len == 0)
  {
    return false;
  }

  data_ = data;
  len_ = len;
  ptr_ = data_;
  return true;
}

inline void MemoryReader::close() noexcept
{
  data_ = nullptr;
  len_ = 0;
  ptr_ = nullptr;
}

inline bool MemoryReader::is_open() const noexcept
{
  return (data_ != nullptr);
}

inline bool MemoryReader::is_eof() const noexcept
{
  if (data_ == nullptr)
  {
    return true;
  }

  return (ptr_ >= data_ + len_ || ptr_ < data_);
}

inline std::ptrdiff_t MemoryReader::position() const noexcept
{
  if (data_ == nullptr)
  {
    return 0;
  }

  return ptr_ - data_;
}

inline std::size_t MemoryReader::size() const noexcept
{
  return static_cast<std::size_t>(len_);
}

inline std::ptrdiff_t MemoryReader::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : (data_ + len_) - ptr_;
}

inline void MemoryReader::rewind() noexcept
{
  ptr_ = data_;
}

inline bool MemoryReader::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (offset < 0 || offset > len_)
  {
    return false;
  }

  ptr_ = data_ + offset;
  return true;
}

inline bool MemoryReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (ptr_ + offset < data_ || ptr_ + offset >= data_ + len_)
  {
    return false;
  }

  ptr_ += offset;
  return true;
}

template <typename T, typename>
inline bool MemoryReader::read(T& value) noexcept
{
  SELENE_ASSERT(ptr_ != nullptr);

  if (ptr_ + sizeof(T) > data_ + len_)
  {
    return false;
  }

  std::memcpy(value, ptr_, sizeof(T)); // memory access might be unaligned
  ptr_ += sizeof(T);
  return true;
};

template <typename T, typename>
inline std::size_t MemoryReader::read(T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(ptr_ != nullptr);
  const std::ptrdiff_t nr_values_available = (data_ + len_ - ptr_) / sizeof(T);
  auto nr_values_read = std::min(std::max(std::ptrdiff_t(0), nr_values_available),
                                 static_cast<std::ptrdiff_t>(nr_values));
  std::memcpy(values, ptr_, static_cast<std::size_t>(nr_values_read * sizeof(T)));
  ptr_ += nr_values_read * sizeof(T);
  return static_cast<std::size_t>(nr_values_read);
};

// ----------

template <typename T, typename>
T read(MemoryReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
};

template <typename T, typename>
inline bool read(MemoryReader& source, T& value) noexcept
{
  return source.read(value);
};

template <typename T, typename>
inline std::size_t read(MemoryReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
};

} // namespace io
} // namespace selene

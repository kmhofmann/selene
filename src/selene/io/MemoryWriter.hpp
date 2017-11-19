// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>
#include <selene/io/WriterMode.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace selene {
namespace io {

class MemoryWriter
{
public:
  MemoryWriter() = default;
  MemoryWriter(std::uint8_t* data, std::size_t len, WriterMode mode = WriterMode::Write);
  ~MemoryWriter() = default;

  MemoryWriter(const MemoryWriter&) = delete;
  MemoryWriter& operator=(const MemoryWriter&) = delete;
  MemoryWriter(MemoryWriter&&) = default;
  MemoryWriter& operator=(MemoryWriter&&) = default;

  std::uint8_t* handle() noexcept;

  bool open(std::uint8_t* data, std::size_t len, WriterMode mode = WriterMode::Write) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;
  std::size_t size() const noexcept;
  std::ptrdiff_t bytes_remaining() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;
  void flush() noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  bool write(const T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  std::size_t write(const T* values, std::size_t nr_values) noexcept;

private:
  std::uint8_t* data_ = nullptr;
  std::ptrdiff_t len_ = 0;
  std::uint8_t* ptr_ = nullptr;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool write(MemoryWriter& sink, const T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t write(MemoryWriter& sink, const T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline MemoryWriter::MemoryWriter(std::uint8_t* data, std::size_t len, WriterMode mode)
{
  if (!open(data, len, mode))
  {
    throw std::runtime_error("Invalid memory region or append mode");
  }
}

inline std::uint8_t* MemoryWriter::handle() noexcept
{
  return ptr_;
}

inline bool MemoryWriter::open(std::uint8_t* data, std::size_t len, WriterMode mode) noexcept
{
  if (data == nullptr || len == 0 || mode == WriterMode::Append)
  {
    return false;
  }

  data_ = data;
  len_ = len;
  ptr_ = data_;
  return true;
}

inline void MemoryWriter::close() noexcept
{
  data_ = nullptr;
  len_ = 0;
  ptr_ = nullptr;
}

inline bool MemoryWriter::is_open() const noexcept
{
  return (data_ != nullptr);
}

inline bool MemoryWriter::is_eof() const noexcept
{
  if (data_ == nullptr)
  {
    return true;
  }

  return (ptr_ >= data_ + len_ || ptr_ < data_);
}

inline std::ptrdiff_t MemoryWriter::position() const noexcept
{
  if (data_ == nullptr)
  {
    return 0;
  }

  return ptr_ - data_;
}

inline std::size_t MemoryWriter::size() const noexcept
{
  return static_cast<std::size_t>(len_);
}

inline std::ptrdiff_t MemoryWriter::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : (data_ + len_) - ptr_;
}

inline void MemoryWriter::rewind() noexcept
{
  ptr_ = data_;
}

inline bool MemoryWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (offset < 0 || offset > len_)
  {
    return false;
  }

  ptr_ = data_ + offset;
  return true;
}

inline bool MemoryWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (ptr_ + offset < data_ || ptr_ + offset >= data_ + len_)
  {
    return false;
  }

  ptr_ += offset;
  return true;
}

inline void MemoryWriter::flush() noexcept
{
}

template <typename T, typename>
inline bool MemoryWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(ptr_ != nullptr);

  if (ptr_ + sizeof(T) > data_ + len_)
  {
    return false;
  }

  std::memcpy(ptr_, value, sizeof(T)); // memory access might be unaligned
  ptr_ += sizeof(T);
  return true;
}

template <typename T, typename>
inline std::size_t MemoryWriter::write(const T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(ptr_ != nullptr);
  const std::ptrdiff_t nr_values_available = (data_ + len_ - ptr_) / sizeof(T);
  auto nr_values_written = std::min(std::max(std::ptrdiff_t(0), nr_values_available),
                                    static_cast<std::ptrdiff_t>(nr_values));
  std::memcpy(ptr_, values, static_cast<std::size_t>(nr_values_written * sizeof(T)));
  ptr_ += nr_values_written * sizeof(T);
  return static_cast<std::size_t>(nr_values_written);
}

// ----------

template <typename T, typename>
inline bool write(MemoryWriter& sink, const T& value) noexcept
{
  return sink.write(value);
};

template <typename T, typename>
inline std::size_t write(MemoryWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
};

} // namespace io
} // namespace selene

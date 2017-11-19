// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>
#include <selene/io/WriterMode.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

namespace selene {
namespace io {

class VectorWriter
{
public:
  VectorWriter() = default;
  explicit VectorWriter(std::vector<std::uint8_t>& data, WriterMode mode = WriterMode::Write);
  ~VectorWriter() = default;

  VectorWriter(const VectorWriter&) = delete;
  VectorWriter& operator=(const VectorWriter&) = delete;
  VectorWriter(VectorWriter&&) = default;
  VectorWriter& operator=(VectorWriter&&) = default;

  std::vector<std::uint8_t>* handle() noexcept;

  bool open(std::vector<std::uint8_t>& data, WriterMode mode = WriterMode::Write) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;
  void flush() noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
    bool write(const T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
    std::size_t write(const T* values, std::size_t nr_values) noexcept;

private:
  std::vector<std::uint8_t>* data_ = nullptr;
  std::ptrdiff_t pos_ = 0;

  bool write_bytes(const std::uint8_t* ptr, std::size_t len);
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool write(VectorWriter& sink, const T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t write(VectorWriter& sink, const T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline VectorWriter::VectorWriter(std::vector<std::uint8_t>& data, WriterMode mode)
{
  open(data, mode);
}

inline std::vector<std::uint8_t>* VectorWriter::handle() noexcept
{
  return data_;
}

inline bool VectorWriter::open(std::vector<std::uint8_t>& data, WriterMode mode) noexcept
{
  data_ = &data;

  if (mode == WriterMode::Write)
  {
    data_->clear();
  }

  pos_ = static_cast<std::ptrdiff_t>(data_->size());
  return true;
}

inline void VectorWriter::close() noexcept
{
  data_ = nullptr;
  pos_ = 0;
}

inline bool VectorWriter::is_open() const noexcept
{
  return data_ != nullptr;
}

inline bool VectorWriter::is_eof() const noexcept
{
  return !data_;
}

inline std::ptrdiff_t VectorWriter::position() const noexcept
{
  return pos_;
}

inline void VectorWriter::rewind() noexcept
{
  pos_ = 0;
}

inline bool VectorWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (offset < 0 || offset > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = offset;
  return true;
}

inline bool VectorWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  const auto new_pos = pos_ + offset;

  if (new_pos < 0 || new_pos > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = new_pos;
  return true;
}

inline void VectorWriter::flush() noexcept
{
}

template <typename T, typename>
inline bool VectorWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(data_);

  const std::uint8_t* ptr = reinterpret_cast<const std::uint8_t*>(&value);
  const std::size_t len = sizeof(T);
  return write_bytes(ptr, len);
}

template <typename T, typename>
inline std::size_t VectorWriter::write(const T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(data_);

  const std::uint8_t* ptr = reinterpret_cast<const std::uint8_t*>(values);
  const std::size_t len = nr_values * sizeof(T);
  write_bytes(ptr, len);
  return len;
}

inline bool VectorWriter::write_bytes(const std::uint8_t* ptr, std::size_t len)
{
  // write `len` bytes, starting from `ptr`, to the current position;
  // i.e. either overwrite the vector data, or insert at the end
  auto cur_ptr = ptr;
  auto len_remaining = static_cast<std::ptrdiff_t>(len);
  const auto bytes_to_end = static_cast<std::ptrdiff_t>(data_->size()) - pos_;
  const auto bytes_to_copy = std::min(len_remaining, bytes_to_end);

  if (bytes_to_copy > 0)
  {
    std::copy(cur_ptr, cur_ptr + bytes_to_copy, &(*data_)[pos_]);
    cur_ptr += bytes_to_copy; // for the assert below
    len_remaining -= bytes_to_copy;
  }
  if (len_remaining > 0)
  {
    data_->insert(data_->end(), cur_ptr, cur_ptr + len_remaining);
    cur_ptr += len_remaining; // for the assert below
  }

  SELENE_ASSERT(cur_ptr - ptr == static_cast<std::ptrdiff_t>(len));
  pos_ += len;
  return true;
}

// ----------

template <typename T, typename>
inline bool write(VectorWriter& sink, const T& value) noexcept
{
  return sink.write(value);
};

template <typename T, typename>
inline std::size_t write(VectorWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
};

} // namespace io
} // namespace selene

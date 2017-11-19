// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

namespace selene {
namespace io {

class VectorReader
{
public:
  VectorReader() = default;
  explicit VectorReader(const std::vector<std::uint8_t>& data);
  ~VectorReader() = default;

  VectorReader(const VectorReader&) = delete;
  VectorReader& operator=(const VectorReader&) = delete;
  VectorReader(VectorReader&&) = default;
  VectorReader& operator=(VectorReader&&) = default;

  const std::vector<std::uint8_t>* handle() noexcept;

  bool open(const std::vector<std::uint8_t>& data) noexcept;
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
  const std::vector<std::uint8_t>* data_ = nullptr;
  std::ptrdiff_t pos_ = 0;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
T read(VectorReader& source);

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool read(VectorReader& source, T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t read(VectorReader& source, T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline VectorReader::VectorReader(const std::vector<std::uint8_t>& data)
{
  open(data);
}

inline const std::vector<std::uint8_t>* VectorReader::handle() noexcept
{
  return data_;
}

inline bool VectorReader::open(const std::vector<std::uint8_t>& data) noexcept
{
  data_ = &data;
  pos_ = std::ptrdiff_t{0};
  return true;
}

inline void VectorReader::close() noexcept
{
  data_ = nullptr;
  pos_ = 0;
}

inline bool VectorReader::is_open() const noexcept
{
  return data_ != nullptr;
}

inline bool VectorReader::is_eof() const noexcept
{
  return !data_ || pos_ == static_cast<std::ptrdiff_t>(data_->size());
}

inline std::ptrdiff_t VectorReader::position() const noexcept
{
  return pos_;
}

inline std::size_t VectorReader::size() const noexcept
{
  return data_->size();
}

inline std::ptrdiff_t VectorReader::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : std::ptrdiff_t(data_->size()) - pos_;
}

inline void VectorReader::rewind() noexcept
{
  pos_ = 0;
}

inline bool VectorReader::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (offset < 0 || offset > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = offset;
  return true;
}

inline bool VectorReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  const auto new_pos = pos_ + offset;

  if (new_pos < 0 || new_pos > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = new_pos;
  return true;

}

template <typename T, typename>
inline bool VectorReader::read(T& value) noexcept
{
  SELENE_ASSERT(data_);
  SELENE_ASSERT(pos_ <= static_cast<std::ptrdiff_t>(data_->size()));

  const std::size_t len = sizeof(T);
  const auto bytes_to_end = data_->size() - pos_;

  if (bytes_to_end < len)
  {
    return false;
  }

  std::memcpy(&value, data_->data() + pos_, len);
  pos_ += len;
  return true;
};

template <typename T, typename>
inline std::size_t VectorReader::read(T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(data_);
  SELENE_ASSERT(pos_ <= static_cast<std::ptrdiff_t>(data_->size()));

  const std::size_t len = sizeof(T);
  const auto bytes_to_end = data_->size() - pos_;
  const auto available_values = std::min(bytes_to_end / len, nr_values);

  std::memcpy(values, data_->data() + pos_, available_values * len);
  pos_ += available_values * len;
  return available_values;
};

// ----------

template <typename T, typename>
T read(VectorReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
};

template <typename T, typename>
inline bool read(VectorReader& source, T& value) noexcept
{
  return source.read(value);
};

template <typename T, typename>
inline std::size_t read(VectorReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
};

} // namespace io
} // namespace selene

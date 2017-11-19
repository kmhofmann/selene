// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace selene {
namespace io {

class FileReader
{
public:
  FileReader() = default;
  explicit FileReader(const char* filename);
  ~FileReader();

  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader(FileReader&&) = default;
  FileReader& operator=(FileReader&&) = default;

  std::FILE* handle() noexcept;

  bool open(const char* filename) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
    bool read(T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
    std::size_t read(T* values, std::size_t nr_values) noexcept;

private:
  std::FILE* fp_ = nullptr;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
T read(FileReader& source);

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool read(FileReader& source, T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t read(FileReader& source, T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline FileReader::FileReader(const char* filename)
{
  if (!open(filename))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

inline FileReader::~FileReader()
{
  if (fp_)
  {
    std::fclose(fp_);
  }
}

inline std::FILE* FileReader::handle() noexcept
{
  return fp_;
}

inline bool FileReader::open(const char* filename) noexcept
{
  if (is_open())
  {
    close();
  }

  fp_ = std::fopen(filename, "rb");
  return (fp_ != nullptr);
}

inline void FileReader::close() noexcept
{
  if (fp_)
  {
    std::fclose(fp_);
    fp_ = nullptr;
  }
}

inline bool FileReader::is_open() const noexcept
{
  return (fp_ != nullptr);
}

inline bool FileReader::is_eof() const noexcept
{
  if (!fp_)
  {
    return true;
  }

  return std::feof(fp_) != 0;
}

inline std::ptrdiff_t FileReader::position() const noexcept
{
  if (!fp_)
  {
    return std::ptrdiff_t(0);
  }

  return static_cast<std::ptrdiff_t>(std::ftell(fp_));
}

inline void FileReader::rewind() noexcept
{
  if (fp_)
  {
    std::rewind(fp_);
  }
}

inline bool FileReader::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_SET);
  return (rc == 0);
}

inline bool FileReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_CUR);
  return (rc == 0);
}

template <typename T, typename>
inline bool FileReader::read(T& value) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_read = std::fread(&value, sizeof(T), 1, fp_);
  return (nr_values_read == 1);
};

template <typename T, typename>
inline std::size_t FileReader::read(T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_read = std::fread(values, sizeof(T), nr_values, fp_);
  return nr_values_read;
};

// ----------

template <typename T, typename>
T read(FileReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
};

template <typename T, typename>
inline bool read(FileReader& source, T& value) noexcept
{
  return source.read(value);
};

template <typename T, typename>
inline std::size_t read(FileReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
};

} // namespace io
} // namespace selene

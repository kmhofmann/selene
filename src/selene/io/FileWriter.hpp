// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <selene/base/Assert.hpp>
#include <selene/io/WriterMode.hpp>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace selene {
namespace io {

class FileWriter
{
public:
  FileWriter() = default;
  explicit FileWriter(const char* filename, WriterMode mode = WriterMode::Write);
  ~FileWriter();

  FileWriter(const FileWriter&) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter(FileWriter&&) noexcept = default;
  FileWriter& operator=(FileWriter&&) noexcept = default;

  std::FILE* handle() noexcept;

  bool open(const char* filename, WriterMode mode = WriterMode::Write) noexcept;
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
  std::FILE* fp_ = nullptr;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool write(FileWriter& sink, const T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t write(FileWriter& sink, const T* values, std::size_t nr_values) noexcept;

// ----------
// Implementation:

inline FileWriter::FileWriter(const char* filename, WriterMode mode)
{
  if (!open(filename, mode))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

inline FileWriter::~FileWriter()
{
  if (fp_)
  {
    std::fclose(fp_);
  }
}

inline std::FILE* FileWriter::handle() noexcept
{
  return fp_;
}

inline bool FileWriter::open(const char* filename, WriterMode mode) noexcept
{
  if (is_open())
  {
    close();
  }

  fp_ = std::fopen(filename, mode == WriterMode::Append ? "rb+" : "wb");

  if (mode == WriterMode::Append)
  {
    if (!fp_)  // fp_ will be nullptr if the file did not exist above, so try to create it using "wb+"
    {
      fp_ = std::fopen(filename, "wb+");
    }

    std::fseek(fp_, 0, SEEK_END);
  }

  return (fp_ != nullptr);
}

inline void FileWriter::close() noexcept
{
  if (fp_)
  {
    std::fclose(fp_);
    fp_ = nullptr;
  }
}

inline bool FileWriter::is_open() const noexcept
{
  return (fp_ != nullptr);
}

inline bool FileWriter::is_eof() const noexcept
{
  if (!fp_)
  {
    return true;
  }

  return std::feof(fp_) != 0;
}

inline std::ptrdiff_t FileWriter::position() const noexcept
{
  if (!fp_)
  {
    return std::ptrdiff_t(0);
  }

  return static_cast<std::ptrdiff_t>(std::ftell(fp_));
}

inline void FileWriter::rewind() noexcept
{
  if (fp_)
  {
    std::rewind(fp_);
  }
}

inline bool FileWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_SET);
  return (rc == 0);
}

inline bool FileWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_CUR);
  return (rc == 0);
}

inline void FileWriter::flush() noexcept
{
  if (fp_)
  {
    std::fflush(fp_);
  }
}

template <typename T, typename>
inline bool FileWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_written = std::fwrite(&value, sizeof(T), 1, fp_);
  return (nr_values_written == 1);
}

template <typename T, typename>
inline std::size_t FileWriter::write(const T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_written = std::fwrite(values, sizeof(T), nr_values, fp_);
  return nr_values_written;
}

// ----------

template <typename T, typename>
inline bool write(FileWriter& sink, const T& value) noexcept
{
  return sink.write(value);
};

template <typename T, typename>
inline std::size_t write(FileWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
};

} // namespace io
} // namespace selene

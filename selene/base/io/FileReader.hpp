// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_FILE_READER_HPP
#define SELENE_IO_FILE_READER_HPP

/// @file

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <selene/base/Assert.hpp>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace sln {

/** \brief Class for reading binary data from files.
 *
 * Class for reading binary data from files. Provides the usual operations for random file access. As much of the
 * interface as possible is equal to the MemoryReader and VectorReader classes. This enables user code to abstract from
 * the particular type of "reader" by means of static polymorphism (e.g. treating the "reader" type as a template).
 */
class FileReader
{
public:
  FileReader() = default;
  explicit FileReader(const char* filename);
  explicit FileReader(const std::string& filename);
  ~FileReader();

  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader(FileReader&&) noexcept = default;  ///< Move constructor.
  FileReader& operator=(FileReader&&) noexcept = default;  ///< Move assignment operator.

  std::FILE* handle() noexcept;

  bool open(const char* filename) noexcept;
  bool open(const std::string& filename) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;
  bool seek_end(std::ptrdiff_t offset) noexcept;

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

/** \brief Opens the specified file for reading and sets the file stream pointer to the beginning of the file.
 *
 * If the file `filename` can not be opened, the function will throw a `std::runtime_error` exception.
 * See also FileReader::open.
 *
 * \param filename The name of the file to be opened for reading.
 */
inline FileReader::FileReader(const char* filename)
{
  if (!open(filename))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

/** \brief Opens the specified file for reading and sets the file stream pointer to the beginning of the file.
 *
 * If the file `filename` can not be opened, the function will throw a `std::runtime_error` exception.
 * See also FileReader::open.
 *
 * \param filename The name of the file to be opened for reading.
 */
inline FileReader::FileReader(const std::string& filename)
{
  if (!open(filename))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

/** \brief Destructor; closes the previously opened file stream. */
inline FileReader::~FileReader()
{
  if (fp_)
  {
    std::fclose(fp_);
  }
}

/** \brief Returns a native handle to the file stream.
 *
 * \return The native file stream handle in form of a `std::FILE*`.
 *         Will return `nullptr` if no file stream is currently opened.
 */
inline std::FILE* FileReader::handle() noexcept
{
  return fp_;
}

/** \brief Opens the specified file for reading and sets the file stream pointer to the beginning of the file.
 *
 * Any already open file will be closed.
 * Opening a file stream can fail for various reasons, e.g. if the specified file does not exist.
 * The failure cases generally match the failure cases of `std::fopen`.
 *
 * \param filename The name of the file to be opened for reading.
 * \return True, if the file was successfully opened; false otherwise.
 */
inline bool FileReader::open(const char* filename) noexcept
{
  if (is_open())
  {
    close();
  }

  fp_ = std::fopen(filename, "rb");
  return (fp_ != nullptr);
}

/** \brief Opens the specified file for reading and sets the file stream pointer to the beginning of the file.
 *
 * Any already open file will be closed.
 * Opening a file stream can fail for various reasons, e.g. if the specified file does not exist.
 * The failure cases generally match the failure cases of `std::fopen`.
 *
 * \param filename The name of the file to be opened for reading.
 * \return True, if the file was successfully opened; false otherwise.
 */
inline bool FileReader::open(const std::string& filename) noexcept
{
  return open(filename.c_str());
}

/** \brief Closes an open file stream.
 *
 * The function will have no effect, if no file stream is currently opened.
 */
inline void FileReader::close() noexcept
{
  if (fp_)
  {
    std::fclose(fp_);
    fp_ = nullptr;
  }
}

/** \brief Returns whether a file stream is open.
 *
 * \return True, if a file stream is open; false otherwise.
 */
inline bool FileReader::is_open() const noexcept
{
  return (fp_ != nullptr);
}

/** \brief Returns whether the end of the file stream has been reached.
 *
 * \return True, if the end of the file stream has been reached (or if no file stream is open); false otherwise.
 */
inline bool FileReader::is_eof() const noexcept
{
  if (!fp_)
  {
    return true;
  }

  return std::feof(fp_) != 0;
}

/** \brief Returns the current value of the file position indicator.
 *
 * \return The numeric value of the file position indicator, or -1 on failure (also, if no file stream is open).
 */
inline std::ptrdiff_t FileReader::position() const noexcept
{
  if (!fp_)
  {
    return std::ptrdiff_t(-1);
  }

  return static_cast<std::ptrdiff_t>(std::ftell(fp_));
}

/** \brief Resets the file position indicator to the beginning of the file stream.
 *
 * The function will have no effect if no file stream is open.
 */
inline void FileReader::rewind() noexcept
{
  if (fp_)
  {
    std::rewind(fp_);
  }
}

/** \brief Performs an absolute seek operation to the specified offset.
 *
 * The function sets the position indicator for the file stream to the specified offset.
 * Failure cases include no file being open, or the offset being outside the file stream region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool FileReader::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_SET);
  return (rc == 0);
}

/** \brief Performs a relative seek operation by the specified offset.
 *
 * The function moves the position indicator for the file stream by the specified relative offset.
 * Failure cases include no file being open, or the resulting position being outside the file stream region.
 *
 * \param offset The relative offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool FileReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_CUR);
  return (rc == 0);
}

/** \brief Performs an absolute seek operation to the specified offset, relative to the end of the file.
 *
 * The function sets the position indicator for the file stream to the specified offset, relative to the end of the file.
 * Failure cases include no file being open, or the offset being outside the file stream region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool FileReader::seek_end(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_END);
  return (rc == 0);
}

/** \brief Reads an element of type T and writes the element to the output parameter `value`.
 *
 * In generic code, prefer using the corresponding non-member function.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param[out] value An element of type T, if the read operation was successful.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool FileReader::read(T& value) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_read = std::fread(&value, sizeof(T), 1, fp_);
  return (nr_values_read == 1);
}

/** \brief Reads `nr_values` elements of type T and writes the elements to the output parameter `values`.
 *
 * In generic code, prefer using the corresponding non-member function.
 *
 * \tparam T The type of the data elements to be read. Needs to be trivially copyable.
 * \param[out] values A pointer to a memory location where the read elements should be written to.
 * \param nr_values The number of data elements to read.
 * \return The number of data elements that were successfully read.
 */
template <typename T, typename>
inline std::size_t FileReader::read(T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_read = std::fread(values, sizeof(T), nr_values, fp_);
  return nr_values_read;
}

// ----------

/** \brief Reads an element of type T from `source` and returns the element.
 *
 * The function does not perform an explicit check (beyond a debug-mode assertion) whether the requested element was
 * actually read. If the read operation failed, then the returned result is undefined.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source FileReader instance.
 * \return An element of type T, if the read operation was successful.
 */
template <typename T, typename>
T read(FileReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
}

/** \brief Reads an element of type T from `source` and writes the element to the output parameter `value`.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source FileReader instance.
 * \param[out] value An element of type T, if the read operation was successful.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool read(FileReader& source, T& value) noexcept
{
  return source.read(value);
}

/** \brief Reads `nr_values` elements of type T from `source` and writes the elements to the output parameter `values`.
 *
 * \tparam T The type of the data elements to be read. Needs to be trivially copyable.
 * \param source The source FileReader instance.
 * \param[out] values A pointer to a memory location where the read elements should be written to.
 * \param nr_values The number of data elements to read.
 * \return The number of data elements that were successfully read.
 */
template <typename T, typename>
inline std::size_t read(FileReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
}

}  // namespace sln

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif  // SELENE_IO_FILE_READER_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_FILE_WRITER_HPP
#define SELENE_IO_FILE_WRITER_HPP

/// @file

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <selene/base/Assert.hpp>
#include <selene/io/WriterMode.hpp>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace sln {

/** \brief Class for writing binary data to files.
 *
 * Class for writing binary data to files. Provides the usual operations for random file access. As much of the
 * interface as possible is equal to the MemoryWriter and VectorWriter classes. This enables user code to abstract from
 * the particular type of "writer" by means of static polymorphism (e.g. treating the "writer" type as a template).
 */
class FileWriter
{
public:
  FileWriter() = default;
  explicit FileWriter(const char* filename, WriterMode mode = WriterMode::Write);
  explicit FileWriter(const std::string& filename, WriterMode mode = WriterMode::Write);
  ~FileWriter();

  FileWriter(const FileWriter&) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter(FileWriter&&) noexcept = default;  ///< Move constructor.
  FileWriter& operator=(FileWriter&&) noexcept = default;  ///< Move assignment operator.

  std::FILE* handle() noexcept;

  bool open(const char* filename, WriterMode mode = WriterMode::Write) noexcept;
  bool open(const std::string& filename, WriterMode mode = WriterMode::Write) noexcept;
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

/** \brief Opens the specified file for writing.
 *
 * If the file `filename` can not be opened, the function will throw a `std::runtime_error` exception.
 * See also FileWriter::open.
 *
 * \param filename The name of the file to be opened for reading.
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append.
 */
inline FileWriter::FileWriter(const char* filename, WriterMode mode)
{
  if (!open(filename, mode))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

/** \brief Opens the specified file for writing.
 *
 * If the file `filename` can not be opened, the function will throw a `std::runtime_error` exception.
 * See also FileWriter::open.
 *
 * \param filename The name of the file to be opened for reading.
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append.
 */
inline FileWriter::FileWriter(const std::string& filename, WriterMode mode)
{
  if (!open(filename, mode))
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

/** \brief Destructor; closes the previously opened file stream. */
inline FileWriter::~FileWriter()
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
inline std::FILE* FileWriter::handle() noexcept
{
  return fp_;
}

/** \brief Opens the specified file for writing.
 *
 * Any already open file will be closed.
 * Opening a file stream can fail for various reasons. The failure cases generally match the failure cases of
 * `std::fopen`.
 *
 * \param filename The name of the file to be opened for reading.
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append.
 * \return True, if the file was successfully opened; false otherwise.
 */
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

/** \brief Opens the specified file for writing.
 *
 * Any already open file will be closed.
 * Opening a file stream can fail for various reasons. The failure cases generally match the failure cases of
 * `std::fopen`.
 *
 * \param filename The name of the file to be opened for reading.
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append.
 * \return True, if the file was successfully opened; false otherwise.
 */
inline bool FileWriter::open(const std::string& filename, WriterMode mode) noexcept
{
  return open(filename.c_str(), mode);
}

/** \brief Closes an open file stream.
 *
 * The function will have no effect, if no file stream is currently opened.
 */
inline void FileWriter::close() noexcept
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
inline bool FileWriter::is_open() const noexcept
{
  return (fp_ != nullptr);
}

/** \brief Returns whether the end of the file stream has been reached.
 *
 * \return True, if the end of the file stream has been reached (or if no file stream is open); false otherwise.
 */
inline bool FileWriter::is_eof() const noexcept
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
inline std::ptrdiff_t FileWriter::position() const noexcept
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
inline void FileWriter::rewind() noexcept
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
 * \note Seeking beyond the existing end of the file may be legal, depending on platform.
 * In this case, FileWriter::seek_abs will behave similar to `std::fseek`.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool FileWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_SET);
  return (rc == 0);
}

/** \brief Performs a relative seek operation to the specified offset.
 *
 * The function moves the position indicator for the file stream by the specified relative offset.
 * Failure cases include no file being open, or the resulting position being outside the file stream region.
 *
 * \note Seeking beyond the existing end of the file may be legal, depending on platform.
 * In this case, FileWriter::seek_rel will behave similar to `std::fseek`.
 *
 * \param offset The relative offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool FileWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!fp_)
  {
    return false;
  }

  const auto rc = std::fseek(fp_, static_cast<long>(offset), SEEK_CUR);
  return (rc == 0);
}

/** \brief Flushes the file output stream.
 *
 * Writes any unwritten data from the file stream buffer to the associated file.
 */
inline void FileWriter::flush() noexcept
{
  if (fp_)
  {
    std::fflush(fp_);
  }
}

/** \brief Writes an element of type T.
 *
 * In generic code, prefer using the corresponding non-member function.
 *
 * \tparam T The type of the data element to be written. Needs to be trivially copyable.
 * \param value The data element to be written.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool FileWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_written = std::fwrite(&value, sizeof(T), 1, fp_);
  return (nr_values_written == 1);
}

/** \brief Writes `nr_values` elements of type T.
 *
 * In generic code, prefer using the corresponding non-member function.
 *
 * \tparam T The type of the data elements to be written. Needs to be trivially copyable.
 * \param[out] values A pointer to the memory location where the elements to be written can be read from.
 * \param nr_values The number of data elements to write.
 * \return The number of data elements that were successfully written.
 */
template <typename T, typename>
inline std::size_t FileWriter::write(const T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(fp_);
  const auto nr_values_written = std::fwrite(values, sizeof(T), nr_values, fp_);
  return nr_values_written;
}

// ----------

/** \brief Writes an element of type T to `sink`.
 *
 * \tparam T The type of the data element to be written. Needs to be trivially copyable.
 * \param sink The sink FileWriter instance.
 * \param value The data element to be written.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool write(FileWriter& sink, const T& value) noexcept
{
  return sink.write(value);
}

/** \brief Writes `nr_values` elements of type T to `sink`.
 *
 * \tparam T The type of the data elements to be written. Needs to be trivially copyable.
 * \param sink The sink FileWriter instance.
 * \param[out] values A pointer to the memory location where the elements to be written can be read from.
 * \param nr_values The number of data elements to write.
 * \return The number of data elements that were successfully written.
 */
template <typename T, typename>
inline std::size_t write(FileWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
}

}  // namespace sln

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif  // SELENE_IO_FILE_WRITER_HPP

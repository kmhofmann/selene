// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_MEMORY_WRITER_HPP
#define SELENE_IO_MEMORY_WRITER_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/io/WriterMode.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace sln {

/** \brief Class for writing binary data to memory.
 *
 * Class for writing binary data to memory. Provides the usual operations for random memory access. As much of the
 * interface as possible is equal to the FileWriter and VectorWriter classes. This enables user code to abstract from
 * the particular type of "writer" by means of static polymorphism (e.g. treating the "writer" type as a template).
 *
 * The sink type, a std::uint8_t*, can not extend its capacity which is given in the constructor and remains fixed.
 * This is the main difference to the VectorWriter, where the sink is a std::vector<std::uint8_t> and can extend its
 * capacity when needed.
 */
class MemoryWriter
{
public:
  MemoryWriter() = default;
  MemoryWriter(std::uint8_t* data, std::size_t len, WriterMode mode = WriterMode::Write);
  ~MemoryWriter() = default;

  MemoryWriter(const MemoryWriter&) = delete;
  MemoryWriter& operator=(const MemoryWriter&) = delete;
  MemoryWriter(MemoryWriter&&) noexcept = default;  ///< Move constructor.
  MemoryWriter& operator=(MemoryWriter&&) noexcept = default;  ///< Move assignment operator.

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

/** \brief Opens the specified memory region for writing.
 *
 * If the open operation fails (e.g. if `nullptr` was passed), the function will throw a `std::runtime_error` exception.
 * See also MemoryWriter::open.
 *
 * \param data A pointer to the beginning of the memory region to be written to.
 * \param len The size in bytes of the memory region to be read.
 * \param mode The writing mode, which has to be WriterMode::Write (otherwise the open operation will fail).
 */
inline MemoryWriter::MemoryWriter(std::uint8_t* data, std::size_t len, WriterMode mode)
{
  if (!open(data, len, mode))
  {
    throw std::runtime_error("Invalid memory region or append mode");
  }
}

/** \brief Returns a native handle to the memory region.
 *
 * \return The native memory handle in form of a `std::uint8_t`. The returned handle will point to the current internal
 *         read position, not to the beginning of the memory region as specified in the constructor.
 *         Will return `nullptr` if no memory region is currently opened.
 */
inline std::uint8_t* MemoryWriter::handle() noexcept
{
  return ptr_;
}

/** \brief Opens the specified memory region for writing.
 *
 * Any already open memory region will be closed.
 * If `nullptr` is passed to `data`, a length of 0 is passed to `len`, or if `WriterMode::Append` is passed as writing
 * mode, the open operation will fail.
 *
 * \param data A pointer to the beginning of the memory region to be written to.
 * \param len The size in bytes of the memory region to be read.
 * \param mode The writing mode, which has to be WriterMode::Write (otherwise the open operation will fail).
 * \return True, if the memory region was successfully opened; false otherwise.
 */
inline bool MemoryWriter::open(std::uint8_t* data, std::size_t len, WriterMode mode) noexcept
{
  if (data == nullptr || len == 0 || mode == WriterMode::Append)
  {
    return false;
  }

  data_ = data;
  len_ = static_cast<std::ptrdiff_t>(len);
  ptr_ = data_;
  return true;
}

/** \brief Closes an open memory region.
 *
 * The function will have no effect, if no memory region is currently opened.
 */
inline void MemoryWriter::close() noexcept
{
  data_ = nullptr;
  len_ = 0;
  ptr_ = nullptr;
}

/** \brief Returns whether a memory region is open.
 *
 * \return True, if a memory region is open; false otherwise.
 */
inline bool MemoryWriter::is_open() const noexcept
{
  return (data_ != nullptr);
}

/** \brief Returns whether the end of the memory region has been reached.
 *
 * \return True, if the internal memory pointer is outside of the memory region (or if no memory region is open);
 * false otherwise.
 */
inline bool MemoryWriter::is_eof() const noexcept
{
  if (data_ == nullptr)
  {
    return true;
  }

  return (ptr_ >= data_ + len_ || ptr_ < data_);
}

/** \brief Returns the current value of the position inside the memory region.
 *
 * \return The numeric value of the position inside the memory region, or -1 on failure (also, if no memory region is
 *         open).
 */
inline std::ptrdiff_t MemoryWriter::position() const noexcept
{
  if (data_ == nullptr)
  {
    return -1;
  }

  return ptr_ - data_;
}

/** \brief Returns the total size of the memory region.
 *
 * \return The total size of the memory region in bytes, irrespective of the current position inside the memory region.
 */
inline std::size_t MemoryWriter::size() const noexcept
{
  return static_cast<std::size_t>(len_);
}

/** \brief Returns the remaining data size that can still be written to.
 *
 * \return The size in bytes of the remaining data that can still be written to.
 */
inline std::ptrdiff_t MemoryWriter::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : (data_ + len_) - ptr_;
}

/** \brief Resets the current memory position to the beginning of the memory region.
 *
 * The function will have no effect if no memory region is open.
 */
inline void MemoryWriter::rewind() noexcept
{
  ptr_ = data_;
}

/** \brief Performs an absolute seek operation to the specified offset.
 *
 * The function sets the internal memory region pointer to the specified offset.
 * Failure cases include no memory region being open, or the offset being outside the memory region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool MemoryWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset < 0 || offset > len_)
  {
    return false;
  }

  ptr_ = data_ + offset;
  return true;
}

/** \brief Performs a relative seek operation by the specified offset.
 *
 * The function moves the internal memory region pointer by the specified relative offset.
 * Failure cases include no memory region being open, or the resulting position being outside the memory region.
 *
 * \param offset The relative offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool MemoryWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || ptr_ + offset < data_ || ptr_ + offset >= data_ + len_)
  {
    return false;
  }

  ptr_ += offset;
  return true;
}

/** \brief Performs a flush operation.
 *
 * MemoryWriter::flush is a no-op, since writing to memory is not buffered.
 * The function exists for interface completeness.
 */
inline void MemoryWriter::flush() noexcept
{
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
inline bool MemoryWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(ptr_ != nullptr);

  if (ptr_ + sizeof(T) > data_ + len_)
  {
    return false;
  }

  std::memcpy(ptr_, value, sizeof(T));  // memory access might be unaligned
  ptr_ += sizeof(T);
  return true;
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

/** \brief Writes an element of type T to `sink`.
 *
 * \tparam T The type of the data element to be written. Needs to be trivially copyable.
 * \param sink The sink MemoryWriter instance.
 * \param value The data element to be written.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool write(MemoryWriter& sink, const T& value) noexcept
{
  return sink.write(value);
}

/** \brief Writes `nr_values` elements of type T to `sink`.
 *
 * \tparam T The type of the data elements to be written. Needs to be trivially copyable.
 * \param sink The sink MemoryWriter instance.
 * \param[out] values A pointer to the memory location where the elements to be written can be read from.
 * \param nr_values The number of data elements to write.
 * \return The number of data elements that were successfully written.
 */
template <typename T, typename>
inline std::size_t write(MemoryWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
}

}  // namespace sln

#endif  // SELENE_IO_MEMORY_WRITER_HPP

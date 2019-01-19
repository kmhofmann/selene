// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_MEMORY_READER_HPP
#define SELENE_IO_MEMORY_READER_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/io/MemoryRegion.hpp>
#include <selene/base/io/_impl/MemoryFunctions.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace sln {

/// \addtogroup group-base-io
/// @{

/** \brief Class for reading binary data from memory.
 *
 * Class for reading binary data from memory. Provides the usual operations for random memory access. As much of the
 * interface as possible is equal to the FileReader and VectorReader classes. This enables user code to abstract from
 * the particular type of "reader" by means of static polymorphism (e.g. treating the "reader" type as a template).
 */
class MemoryReader
{
public:
  MemoryReader() = default;
  MemoryReader(const ConstantMemoryRegion region);
  ~MemoryReader() = default;

  MemoryReader(const MemoryReader&) = delete;
  MemoryReader& operator=(const MemoryReader&) = delete;
  MemoryReader(MemoryReader&&) noexcept = default;  ///< Move constructor.
  MemoryReader& operator=(MemoryReader&&) noexcept = default;  ///< Move assignment operator.

  const std::uint8_t* handle() noexcept;

  bool open(const ConstantMemoryRegion region) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;
  std::size_t size() const noexcept;
  std::ptrdiff_t bytes_remaining() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;
  bool seek_end(std::ptrdiff_t offset) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  bool read(T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
  std::size_t read(T* values, std::size_t nr_values) noexcept;

private:
  const std::uint8_t* data_ = nullptr;
  std::ptrdiff_t len_ = 0;
  const std::uint8_t* ptr_ = nullptr;
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
T read(MemoryReader& source);

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
bool read(MemoryReader& source, T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
std::size_t read(MemoryReader& source, T* values, std::size_t nr_values) noexcept;

/// @}

// ----------
// Implementation:

/** \brief Opens the specified memory region for reading and sets the read pointer to the beginning of the region.
 *
 * If the open operation fails (e.g. if `nullptr` was passed), the function will throw a `std::runtime_error` exception.
 * See also MemoryReader::open.
 *
 * \param data A pointer to the beginning of the memory region to be read.
 * \param len The size in bytes of the memory region to be read.
 */
inline MemoryReader::MemoryReader(const ConstantMemoryRegion region)
{
  if (!open(region))
  {
    throw std::runtime_error("Invalid memory region");
  }
}

/** \brief Returns a native handle to the memory region.
 *
 * \return The native memory handle in form of a `std::uint8_t`. The returned handle will point to the current internal
 *         read position, not to the beginning of the memory region as specified in the constructor.
 *         Will return `nullptr` if no memory region is currently opened.
 */
inline const std::uint8_t* MemoryReader::handle() noexcept
{
  return ptr_;
}

/** \brief Opens the specified memory region for reading and sets the read pointer to the beginning of the region.
 *
 * Any already open memory region will be closed.
 * If `nullptr` is passed to `data`, or a length of 0 is passed to `len`, the open operation will fail.
 *
 * \param data A pointer to the beginning of the memory region to be read.
 * \param len The size in bytes of the memory region to be read.
 * \return True, if the memory region was successfully opened; false otherwise.
 */
inline bool MemoryReader::open(const ConstantMemoryRegion region) noexcept
{
  if (region.data == nullptr || region.len == 0)
  {
    return false;
  }

  data_ = region.data;
  len_ = static_cast<std::ptrdiff_t>(region.len);
  ptr_ = data_;
  return true;
}

/** \brief Closes an open memory region.
 *
 * The function will have no effect, if no memory region is currently opened.
 */
inline void MemoryReader::close() noexcept
{
  data_ = nullptr;
  len_ = 0;
  ptr_ = nullptr;
}

/** \brief Returns whether a memory region is open.
 *
 * \return True, if a memory region is open; false otherwise.
 */
inline bool MemoryReader::is_open() const noexcept
{
  return (data_ != nullptr);
}

/** \brief Returns whether the end of the memory region has been reached.
 *
 * \return True, if the internal memory pointer is outside of the memory region (or if no memory region is open);
 * false otherwise.
 */
inline bool MemoryReader::is_eof() const noexcept
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
inline std::ptrdiff_t MemoryReader::position() const noexcept
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
inline std::size_t MemoryReader::size() const noexcept
{
  return static_cast<std::size_t>(len_);
}

/** \brief Returns the remaining data size that can still be read.
 *
 * \return The size in bytes of the remaining data that can still be read.
 */
inline std::ptrdiff_t MemoryReader::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : (data_ + len_) - ptr_;
}

/** \brief Resets the current memory position to the beginning of the memory region.
 *
 * The function will have no effect if no memory region is open.
 */
inline void MemoryReader::rewind() noexcept
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
inline bool MemoryReader::seek_abs(std::ptrdiff_t offset) noexcept
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
inline bool MemoryReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || ptr_ + offset < data_ || ptr_ + offset >= data_ + len_)
  {
    return false;
  }

  ptr_ += offset;
  return true;
}

/** \brief Performs an absolute seek operation to the specified offset, relative to the end of the memory region.
 *
 * The function sets the internal memory region pointer to the specified offset, relative to the end of the memory region.
 * Failure cases include no memory region being open, or the offset being outside the memory region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool MemoryReader::seek_end(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset > 0 || offset < -len_)
  {
    return false;
  }

  ptr_ = data_ + len_ + offset;
  return true;
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
inline bool MemoryReader::read(T& value) noexcept
{
  return impl::memory_read_value<T>(data_, len_, ptr_, value);
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
inline std::size_t MemoryReader::read(T* values, std::size_t nr_values) noexcept
{
  return impl::memory_read_values<T>(data_, len_, ptr_, values, nr_values);
}

// ----------

/** \brief Reads an element of type T from `source` and returns the element.
 *
 * The function does not perform an explicit check (beyond a debug-mode assertion) whether the requested element was
 * actually read. If the read operation failed, then the returned result is undefined.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source MemoryReader instance.
 * \return An element of type T, if the read operation was successful.
 */
template <typename T, typename>
T read(MemoryReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
}

/** \brief Reads an element of type T from `source` and writes the element to the output parameter `value`.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source MemoryReader instance.
 * \param[out] value An element of type T, if the read operation was successful.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool read(MemoryReader& source, T& value) noexcept
{
  return source.read(value);
}

/** \brief Reads `nr_values` elements of type T from `source` and writes the elements to the output parameter `values`.
 *
 * \tparam T The type of the data elements to be read. Needs to be trivially copyable.
 * \param source The source MemoryReader instance.
 * \param[out] values A pointer to a memory location where the read elements should be written to.
 * \param nr_values The number of data elements to read.
 * \return The number of data elements that were successfully read.
 */
template <typename T, typename>
inline std::size_t read(MemoryReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
}

}  // namespace sln

#endif  // SELENE_IO_MEMORY_READER_HPP

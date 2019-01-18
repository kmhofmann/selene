// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_VECTOR_READER_HPP
#define SELENE_IO_VECTOR_READER_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/io/_impl/VectorFunctions.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

namespace sln {

/** \brief Class for reading binary data from memory, stored in a std::vector<std::uint8_t>.
 *
 * Class for reading binary data from memory, stored in a std::vector<std::uint8_t>. Provides the usual operations for
 * random memory access. As much of the interface as possible is equal to the FileReader and MemoryReader classes. This
 * enables user code to abstract from the particular type of "reader" by means of static polymorphism (e.g. treating
 * the "reader" type as a template).
 */
class VectorReader
{
public:
  VectorReader() = default;
  explicit VectorReader(const std::vector<std::uint8_t>& data);
  ~VectorReader() = default;

  VectorReader(const VectorReader&) = delete;
  VectorReader& operator=(const VectorReader&) = delete;
  VectorReader(VectorReader&&) noexcept = default;  ///< Move constructor.
  VectorReader& operator=(VectorReader&&) noexcept = default;  ///< Move assignment operator.

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
  bool seek_end(std::ptrdiff_t offset) noexcept;

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

/** \brief Opens the specified vector for reading and sets the read pointer to the beginning of the vector.
 *
 * See also VectorReader::open.
 *
 * \param data The `std::vector<std::uint8_t>`, containing the data to be read.
 */
inline VectorReader::VectorReader(const std::vector<std::uint8_t>& data)
{
  open(data);
}

/** \brief Returns a native handle to the vector.
 *
 * \return The native vector handle in form of a `const std::vector<std::uint8_t>*`.
 *         Will return `nullptr` if no vector is currently opened.
 */
inline const std::vector<std::uint8_t>* VectorReader::handle() noexcept
{
  return data_;
}

/** \brief Opens the specified vector for reading and sets the read pointer to the beginning of the vector.
 *
 * Any already open vector will be closed.
 * Currently, no failure conditions exist for opening of a vector, so the function will always return `true`.
 *
 * \param data The `std::vector<std::uint8_t>`, containing the data to be read.
 * \return True, if the vector was successfully opened; false otherwise.
 */
inline bool VectorReader::open(const std::vector<std::uint8_t>& data) noexcept
{
  data_ = &data;
  pos_ = std::ptrdiff_t{0};
  return true;
}

/** \brief Closes an open vector.
 *
 * The function will have no effect, if no vector is currently opened.
 */
inline void VectorReader::close() noexcept
{
  data_ = nullptr;
  pos_ = 0;
}

/** \brief Returns whether a vector is open.
 *
 * \return True, if a vector is open; false otherwise.
 */
inline bool VectorReader::is_open() const noexcept
{
  return data_ != nullptr;
}

/** \brief Returns whether the end of the vector has been reached.
 *
 * \return True, if the internal vector pointer is outside of the vector data region (or if no vector is open);
 * false otherwise.
 */
inline bool VectorReader::is_eof() const noexcept
{
  return !data_ || pos_ >= static_cast<std::ptrdiff_t>(data_->size()) || pos_ < std::ptrdiff_t{0};
}

/** \brief Returns the current value of the position inside the vector data region.
 *
 * \return The numeric value of the position inside the vector data region, or -1 on failure (also, if no memory region
 *         is open).
 */
inline std::ptrdiff_t VectorReader::position() const noexcept
{
  if (data_ == nullptr)
  {
    return -1;
  }

  return pos_;
}

/** \brief Returns the total size of the vector data region.
 *
 * \return The total size of the vector data region in bytes, irrespective of the current position inside the data
 *         region.
 */
inline std::size_t VectorReader::size() const noexcept
{
  return data_->size();
}

/** \brief Returns the remaining data size that can still be read.
 *
 * \return The size in bytes of the remaining data that can still be read.
 */
inline std::ptrdiff_t VectorReader::bytes_remaining() const noexcept
{
  return is_eof() ? 0 : std::ptrdiff_t(data_->size()) - pos_;
}

/** \brief Resets the current position inside the vector data to the beginning of the data region.
 *
 * The function will have no effect if no vector is open.
 */
inline void VectorReader::rewind() noexcept
{
  pos_ = 0;
}

/** \brief Performs an absolute seek operation to the specified offset.
 *
 * The function sets the internal data pointer to the specified offset.
 * Failure cases include no vector being open, or the offset being outside the vector data region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorReader::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset < 0 || offset > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = offset;
  return true;
}

/** \brief Performs a relative seek operation by the specified offset.
 *
 * The function moves the internal data pointer by the specified relative offset.
 * Failure cases include no vector being open, or the resulting position being outside the vector data region.
 *
 * \param offset The relative offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorReader::seek_rel(std::ptrdiff_t offset) noexcept
{
  const auto new_pos = pos_ + offset;

  if (!is_open() || new_pos < 0 || new_pos > static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = new_pos;
  return true;
}

/** \brief Performs an absolute seek operation to the specified offset, relative to the end of the vector.
 *
 * The function sets the internal data pointer to the specified offset, relative to the end of the vector.
 * Failure cases include no vector being open, or the offset being outside the vector data region.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorReader::seek_end(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset > 0 || offset < -static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  pos_ = static_cast<std::ptrdiff_t>(data_->size()) + offset;
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
inline bool VectorReader::read(T& value) noexcept
{
  return impl::vector_read_value<T>(data_, pos_, value);
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
inline std::size_t VectorReader::read(T* values, std::size_t nr_values) noexcept
{
  return impl::vector_read_values<T>(data_, pos_, values, nr_values);
}

// ----------

/** \brief Reads an element of type T from `source` and returns the element.
 *
 * The function does not perform an explicit check (beyond a debug-mode assertion) whether the requested element was
 * actually read. If the read operation failed, then the returned result is undefined.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source VectorReader instance.
 * \return An element of type T, if the read operation was successful.
 */
template <typename T, typename>
T read(VectorReader& source)
{
  T value{};
  [[maybe_unused]] bool read = source.read(value);
  SELENE_ASSERT(read);
  return value;
}

/** \brief Reads an element of type T from `source` and writes the element to the output parameter `value`.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param source The source VectorReader instance.
 * \param[out] value An element of type T, if the read operation was successful.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool read(VectorReader& source, T& value) noexcept
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
inline std::size_t read(VectorReader& source, T* values, std::size_t nr_values) noexcept
{
  return source.read(values, nr_values);
}

}  // namespace sln

#endif  // SELENE_IO_VECTOR_READER_HPP

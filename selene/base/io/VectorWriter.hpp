// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_VECTOR_WRITER_HPP
#define SELENE_IO_VECTOR_WRITER_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/io/WriterMode.hpp>
#include <selene/base/io/_impl/VectorFunctions.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

namespace sln {

/// \addtogroup group-base-io
/// @{

/** \brief Class for writing binary data to memory, stored in a std::vector<std::uint8_t>.
 *
 * Class for writing binary data to memory, stored in a std::vector<std::uint8_t>. Provides the usual operations for
 * random memory access. As much of the interface as possible is equal to the FileWriter and MemoryWriter classes. This
 * enables user code to abstract from the particular type of "writer" by means of static polymorphism (e.g. treating
 * the "writer" type as a template).
 *
 * The sink type, std::vector<std::uint8_t> will automatically extend when its capacity is exceeded, which is the main
 * difference to the MemoryWriter; here, the sink type is a raw pointer with fixed memory length.
 */
class VectorWriter
{
public:
  VectorWriter() = default;
  explicit VectorWriter(std::vector<std::uint8_t>& data, WriterMode mode = WriterMode::Write);
  ~VectorWriter() = default;

  VectorWriter(const VectorWriter&) = delete;
  VectorWriter& operator=(const VectorWriter&) = delete;
  VectorWriter(VectorWriter&&) noexcept = default;  ///< Move constructor.
  VectorWriter& operator=(VectorWriter&&) noexcept = default;  ///< Move assignment operator.

  std::vector<std::uint8_t>* handle() noexcept;

  bool open(std::vector<std::uint8_t>& data, WriterMode mode = WriterMode::Write) noexcept;
  void close() noexcept;

  bool is_open() const noexcept;
  bool is_eof() const noexcept;
  std::ptrdiff_t position() const noexcept;

  void rewind() noexcept;
  bool seek_abs(std::ptrdiff_t offset) noexcept;
  bool seek_rel(std::ptrdiff_t offset) noexcept;
  bool seek_end(std::ptrdiff_t offset) noexcept;
  void flush() noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  bool read(T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  std::size_t read(T* values, std::size_t nr_values) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  bool write(const T& value) noexcept;

  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  std::size_t write(const T* values, std::size_t nr_values) noexcept;

private:
  std::vector<std::uint8_t>* data_ = nullptr;
  std::ptrdiff_t pos_ = 0;

  bool write_bytes(const std::uint8_t* ptr, std::size_t len);
};

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
T read(VectorWriter& sink);

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
bool read(VectorWriter& sink, T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
std::size_t read(VectorWriter& sink, T* values, std::size_t nr_values) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
bool write(VectorWriter& sink, const T& value) noexcept;

template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
std::size_t write(VectorWriter& sink, const T* values, std::size_t nr_values) noexcept;

/// @}

// ----------
// Implementation:

/** \brief Opens the specified vector for writing.
 *
 * See also VectorWriter::open.
 *
 * \param data The `std::vector<std::uint8_t>`, containing the data to be read.
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append. WriteMode::Write effectively clears the
 *             vector, while WriterMode::Append preserves its contents.
 */
inline VectorWriter::VectorWriter(std::vector<std::uint8_t>& data, WriterMode mode)
{
  open(data, mode);
}

/** \brief Returns a native handle to the vector.
 *
 * \return The native vector handle in form of a `const std::vector<std::uint8_t>*`.
 *         Will return `nullptr` if no vector is currently opened.
 */
inline std::vector<std::uint8_t>* VectorWriter::handle() noexcept
{
  return data_;
}

/** \brief Opens the specified vector for writing.
 *
 * Any already open vector will be closed.
 * Currently, no failure conditions exist for opening of a vector, so the function will always return `true`.
 *
 * \param data The `std::vector<std::uint8_t>`, containing the data to be read
 * \param mode The writing mode, WriterMode::Write or WriterMode::Append. WriteMode::Write effectively clears the
 *             vector, while WriterMode::Append preserves its contents.
 * \return True, if the vector was successfully opened; false otherwise.
 */
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

/** \brief Closes an open vector.
 *
 * The function will have no effect, if no vector is currently opened.
 */
inline void VectorWriter::close() noexcept
{
  data_ = nullptr;
  pos_ = 0;
}

/** \brief Returns whether a vector is open.
 *
 * \return True, if a vector is open; false otherwise.
 */
inline bool VectorWriter::is_open() const noexcept
{
  return data_ != nullptr;
}

/** \brief Returns whether the end of the vector has been reached.
 *
 * Since the vector extends its own capacity, there is no state in which the end of its data region can be reached.
 * Therefore the function generally returns true, unless no vector is open.
 *
 * \return True, if no vector is open; false otherwise.
 */
inline bool VectorWriter::is_eof() const noexcept
{
  return !data_;
}

/** \brief Returns the current value of the position inside the vector data region.
 *
 * \return The numeric value of the position inside the vector data region, or -1 on failure (also, if no memory region
 *         is open).
 */
inline std::ptrdiff_t VectorWriter::position() const noexcept
{
  if (data_ == nullptr)
  {
    return -1;
  }

  return pos_;
}

/** \brief Resets the current position inside the vector data to the beginning of the data region.
 *
 * The function will have no effect if no vector is open.
 */
inline void VectorWriter::rewind() noexcept
{
  pos_ = 0;
}

/** \brief Performs an absolute seek operation to the specified offset.
 *
 * The function sets the internal data pointer to the specified offset.
 * Failure cases include no vector being open, or the offset being negative.
 *
 * If the offset points beyond the current vector size, then the vector will be extended to accommodate the offset.
 * The newly added elements will be `0`.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorWriter::seek_abs(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset < 0)
  {
    return false;
  }

  if (offset > static_cast<std::ptrdiff_t>(data_->size()))
  {
    data_->resize(static_cast<std::size_t>(offset), std::uint8_t{0});
  }

  pos_ = offset;
  return true;
}

/** \brief Performs a relative seek operation by the specified offset.
 *
 * The function moves the internal data pointer by the specified relative offset.
 * Failure cases include no vector being open, or the resulting position being outside the vector data region.
 *
 * If the new absolute position points beyond the current vector size, then the vector will be extended to accommodate
 * the offset. The newly added elements will be `0`.
 *
 * \param offset The relative offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorWriter::seek_rel(std::ptrdiff_t offset) noexcept
{
  const auto new_pos = pos_ + offset;

  if (!is_open() || new_pos < 0)
  {
    return false;
  }

  if (new_pos > static_cast<std::ptrdiff_t>(data_->size()))
  {
    data_->resize(static_cast<std::size_t>(new_pos), std::uint8_t{0});
  }

  pos_ = new_pos;
  return true;
}

/** \brief Performs an absolute seek operation to the specified offset, relative to the end of the vector.
 *
 * The function sets the internal data pointer to the specified offset, relative to the end of the vector.
 * Failure cases include no vector being open, or the offset being less than the negative vector data size (in which
 * case it would point at "negative" elements of the vector).
 *
 * If the offset points beyond the current vector size (i.e. is greater than zero), then the vector will be extended
 * to accommodate the offset. The newly added elements will be `0`.
 *
 * \param offset The absolute offset in bytes.
 * \return True, if the seek operation was successful; false on failure.
 */
inline bool VectorWriter::seek_end(std::ptrdiff_t offset) noexcept
{
  if (!is_open() || offset < -static_cast<std::ptrdiff_t>(data_->size()))
  {
    return false;
  }

  if (offset > 0)
  {
    data_->resize(data_->size() + static_cast<std::size_t>(offset), std::uint8_t{0});
  }

  pos_ = static_cast<std::ptrdiff_t>(data_->size()) + offset;
  return true;
}

/** \brief Performs a flush operation.
 *
 * VectorWriter::flush is a no-op, since writing to a vector is not buffered.
 * The function exists for interface completeness.
 */
inline void VectorWriter::flush() noexcept
{
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
inline bool VectorWriter::read(T& value) noexcept
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
inline std::size_t VectorWriter::read(T* values, std::size_t nr_values) noexcept
{
  return impl::vector_read_values<T>(data_, pos_, values, nr_values);
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
inline bool VectorWriter::write(const T& value) noexcept
{
  SELENE_ASSERT(data_);

  const std::uint8_t* ptr = reinterpret_cast<const std::uint8_t*>(&value);
  const std::size_t len = sizeof(T);
  return write_bytes(ptr, len);
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
inline std::size_t VectorWriter::write(const T* values, std::size_t nr_values) noexcept
{
  SELENE_ASSERT(data_);

  const auto ptr = reinterpret_cast<const std::uint8_t*>(values);
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
    std::copy(cur_ptr, cur_ptr + bytes_to_copy, &(*data_)[static_cast<std::size_t>(pos_)]);
    cur_ptr += bytes_to_copy;  // for the assert below
    len_remaining -= bytes_to_copy;
  }
  if (len_remaining > 0)
  {
    data_->insert(data_->end(), cur_ptr, cur_ptr + len_remaining);
    cur_ptr += len_remaining;  // for the assert below
  }

  SELENE_ASSERT(cur_ptr - ptr == static_cast<std::ptrdiff_t>(len));
  pos_ += len;
  return true;
}

// ----------


/** \brief Reads an element of type T from `sink` and returns the element.
 *
 * The function does not perform an explicit check (beyond a debug-mode assertion) whether the requested element was
 * actually read. If the read operation failed, then the returned result is undefined.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param sink The sink VectorWriter instance.
 * \return An element of type T, if the read operation was successful.
 */
template <typename T, typename>
T read(VectorWriter& sink)
{
  T value{};
  [[maybe_unused]] bool read = sink.read(value);
  SELENE_ASSERT(read);
  return value;
}

/** \brief Reads an element of type T from `sink` and writes the element to the output parameter `value`.
 *
 * \tparam T The type of the data element to be read. Needs to be trivially copyable.
 * \param sink The sink VectorWriter instance.
 * \param[out] value An element of type T, if the read operation was successful.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool read(VectorWriter& sink, T& value) noexcept
{
  return sink.read(value);
}

/** \brief Reads `nr_values` elements of type T from `sink` and writes the elements to the output parameter `values`.
 *
 * \tparam T The type of the data elements to be read. Needs to be trivially copyable.
 * \param sink The sink FileReader instance.
 * \param[out] values A pointer to a memory location where the read elements should be written to.
 * \param nr_values The number of data elements to read.
 * \return The number of data elements that were successfully read.
 */
template <typename T, typename>
inline std::size_t read(VectorWriter& sink, T* values, std::size_t nr_values) noexcept
{
  return sink.read(values, nr_values);
}

/** \brief Writes an element of type T to `sink`.
 *
 * \tparam T The type of the data element to be written. Needs to be trivially copyable.
 * \param sink The sink VectorWriter instance.
 * \param value The data element to be written.
 * \return True, if read operation was successful, false otherwise.
 */
template <typename T, typename>
inline bool write(VectorWriter& sink, const T& value) noexcept
{
  return sink.write(value);
}

/** \brief Writes `nr_values` elements of type T to `sink`.
 *
 * \tparam T The type of the data elements to be written. Needs to be trivially copyable.
 * \param sink The sink VectorWriter instance.
 * \param[out] values A pointer to the memory location where the elements to be written can be read from.
 * \param nr_values The number of data elements to write.
 * \return The number of data elements that were successfully written.
 */
template <typename T, typename>
inline std::size_t write(VectorWriter& sink, const T* values, std::size_t nr_values) noexcept
{
  return sink.write(values, nr_values);
}

}  // namespace sln

#endif  // SELENE_IO_VECTOR_WRITER_HPP

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_ALLOCATORS_HPP
#define SELENE_BASE_ALLOCATORS_HPP

#include <cstdint>
#include <cstdlib>

namespace selene {

class MemoryBlock;

/** \brief Provides default means for memory allocation and deallocation throughout the library.
 *
 *  Used in various places inside the library. Not recommended for memory management outside of the library.
 */
struct DefaultAllocator
{
  static MemoryBlock allocate(std::size_t nr_bytes);
  static void deallocate(std::uint8_t*& data);
};

/** \brief Represents a contiguous block of memory, specified by a pointer to its beginning, and by its size.
 *
 * Memory blocks are returned by various (allocating) functions inside the library; these functions are mostly
 * internal and not user-facing.
 * Memory allocated by a MemoryBlock shall be managed using the DefaultAllocator functions.
 * A MemoryBlock instance that goes out of scope will deallocate the memory it is holding.
 * Empty memory blocks are usually designated by pointing to nullptr and having size 0.
 */
class MemoryBlock
{
public:
  ~MemoryBlock();

  MemoryBlock(const MemoryBlock&) = delete;
  MemoryBlock& operator=(const MemoryBlock&) = delete;

  MemoryBlock(MemoryBlock&& other) noexcept;
  MemoryBlock& operator=(MemoryBlock&& other) noexcept;

  std::uint8_t* data() const;
  std::size_t size() const;

  std::uint8_t* transfer_data();

private:
  std::uint8_t* data_;
  std::size_t size_;

  MemoryBlock(std::uint8_t* data, std::size_t size);
  friend struct DefaultAllocator;
};


// ----------
// Implementation

/** \brief Allocates the specified number of bytes and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
inline MemoryBlock DefaultAllocator::allocate(std::size_t nr_bytes)
{
  if (nr_bytes == 0)
  {
    return MemoryBlock(nullptr, 0);
  }

  auto ptr = static_cast<std::uint8_t*>(std::malloc(nr_bytes));
  return MemoryBlock(ptr, (ptr == nullptr) ? 0 : nr_bytes);
}

/** \brief Deallocates the previously allocated memory block.
 *
 * \param data A pointer to previously allocated data.
 */
inline void DefaultAllocator::deallocate(std::uint8_t*& data)
{
  if (data != nullptr)
  {
    std::free(data);
  }

  data = nullptr;
}

inline MemoryBlock::MemoryBlock(std::uint8_t* data, std::size_t size)
    : data_(data), size_(size)
{ }

inline MemoryBlock::~MemoryBlock()
{
  if (data_ != nullptr)
  {
    DefaultAllocator::deallocate(data_);
  }
}

/** Move constructor. */
inline MemoryBlock::MemoryBlock(MemoryBlock&& other) noexcept
    : data_(other.data_), size_(other.size_)
{
}

/** Move assignment operator. */
inline MemoryBlock& MemoryBlock::operator=(MemoryBlock&& other) noexcept
{
  data_ = other.data_;
  size_ = other.size_;
  return *this;
}

/** \brief Returns a read-write pointer to the allocated memory.
 *
 * \return Pointer to the allocated memory.
 */
inline std::uint8_t* MemoryBlock::data() const
{
  return data_;
}

/** \brief Returns the size of the allocated memory.
 *
 * \return Size of the allocated memory in bytes.
 */
inline std::size_t MemoryBlock::size() const
{
  return size_;
}

/** \brief Returns (and releases) the data of the MemoryBlock instance, and set the MemoryBlock instance to empty.
 *
 * The function can be used when the MemoryBlock instance should go out of scope, but the memory (pointer) should be
 * retained. The memory will have to be deallocated manually, since it is now not bound to the MemoryBlock instance
 * anymore.
 *
 * \return Pointer to the data, previously contained in the MemoryBlock instance.
 */
inline std::uint8_t* MemoryBlock::transfer_data()
{
  auto data = data_;
  data_ = nullptr;
  size_ = 0;
  return data;
}

} // namespace selene

#endif // SELENE_BASE_ALLOCATORS_HPP

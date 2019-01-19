// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_MEMORY_BLOCK_HPP
#define SELENE_BASE_MEMORY_BLOCK_HPP

/// @file

#include <selene/base/Allocators.hpp>

#include <cstdint>
#include <cstdlib>

namespace sln {

/// \addtogroup group-base
/// @{

template <typename Allocator>
class MemoryBlock;

template <typename Allocator>
MemoryBlock<Allocator> construct_memory_block_from_existing_memory(std::uint8_t* data, std::size_t size) noexcept;

/** \brief Represents a contiguous block of memory, specified by a pointer to its beginning, and by its size.
 *
 * Memory blocks are returned by various (allocating) functions inside the library; these functions are mostly
 * internal and not user-facing.
 * Memory allocated by a MemoryBlock shall be managed using the respective corresponding Allocator functions.
 * A MemoryBlock instance that goes out of scope will deallocate the memory it is holding, using the specified Allocator
 * interface.
 * Empty memory blocks are usually designated by pointing to nullptr and having size 0.
 *
 * \tparam Allocator An allocator to use for deallocation of the memory inside a MemoryBlock instance.
 */
template <typename Allocator>
class MemoryBlock
{
public:
  ~MemoryBlock();

  MemoryBlock(const MemoryBlock&) = delete;
  MemoryBlock& operator=(const MemoryBlock&) = delete;

  MemoryBlock(MemoryBlock&& other) noexcept;
  MemoryBlock& operator=(MemoryBlock&& other) noexcept;

  std::uint8_t* data() const noexcept;
  std::size_t size() const noexcept;

  std::uint8_t* transfer_data() noexcept;

private:
  std::uint8_t* data_;
  std::size_t size_;

  MemoryBlock(std::uint8_t* data, std::size_t size);
  friend MemoryBlock<Allocator> construct_memory_block_from_existing_memory<Allocator>(std::uint8_t*,
                                                                                       std::size_t) noexcept;
};

/// @}

// ----------
// Implementation

template <typename Allocator>
inline MemoryBlock<Allocator>::MemoryBlock(std::uint8_t* data, std::size_t size) : data_(data), size_(size)
{
}

template <typename Allocator>
inline MemoryBlock<Allocator>::~MemoryBlock()
{
  if (data_ != nullptr)
  {
    Allocator::deallocate(data_);
  }
}

/** Move constructor. */
template <typename Allocator>
inline MemoryBlock<Allocator>::MemoryBlock(MemoryBlock<Allocator>&& other) noexcept
    : data_(other.data_), size_(other.size_)
{
}

/** Move assignment operator. */
template <typename Allocator>
inline MemoryBlock<Allocator>& MemoryBlock<Allocator>::operator=(MemoryBlock<Allocator>&& other) noexcept
{
  data_ = other.data_;
  size_ = other.size_;
  return *this;
}

/** \brief Returns a read-write pointer to the allocated memory.
 *
 * \return Pointer to the allocated memory.
 */
template <typename Allocator>
inline std::uint8_t* MemoryBlock<Allocator>::data() const noexcept
{
  return data_;
}

/** \brief Returns the size of the allocated memory.
 *
 * \return Size of the allocated memory in bytes.
 */
template <typename Allocator>
inline std::size_t MemoryBlock<Allocator>::size() const noexcept
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
template <typename Allocator>
inline std::uint8_t* MemoryBlock<Allocator>::transfer_data() noexcept
{
  auto data = data_;
  data_ = nullptr;
  size_ = 0;
  return data;
}

/** \brief Constructs a `MemoryBlock<Allocator>` instance from existing memory.
 *
 * It is important that the supplied memory was allocated with a compatible allocation function; otherwise, the
 * deallocation is undefined behavior.
 *
 * @tparam Allocator The allocator type that was used to construct the existing memory region.
 * @param data Pointer to the beginning of the memory region.
 * @param size Size of the memory region.
 * @return A `MemoryBlock<Allocator>` instance.
 */
template <typename Allocator>
inline MemoryBlock<Allocator> construct_memory_block_from_existing_memory(std::uint8_t* data, std::size_t size) noexcept
{
  return MemoryBlock<Allocator>(data, size);
}

}  // namespace sln

#endif  // SELENE_BASE_MEMORY_BLOCK_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/Allocators.hpp>
#include <selene/base/MemoryBlock.hpp>
#include <selene/base/Utils.hpp>

#include <memory>
#include <new>

namespace sln {

/** \brief Allocates the specified number of bytes via `std::malloc` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<MallocAllocator> MallocAllocator::allocate(std::size_t nr_bytes) noexcept
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<MallocAllocator>(nullptr, 0);
  }

  auto ptr = static_cast<std::uint8_t*>(std::malloc(nr_bytes));
  return construct_memory_block_from_existing_memory<MallocAllocator>(ptr, (ptr == nullptr) ? 0 : nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `std::free`.
 *
 * \param data A pointer to previously allocated data.
 */
void MallocAllocator::deallocate(std::uint8_t*& data) noexcept
{
  std::free(data);
  data = nullptr;
}


/** \brief Allocates the specified number of bytes via `std::malloc` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \param alignment The required byte alignment. Needs to be a power of two (e.g. 8, 16, 32, ...).
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<AlignedMallocAllocator> AlignedMallocAllocator::allocate(std::size_t nr_bytes,
                                                                     std::size_t alignment) noexcept
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(nullptr, 0);
  }

  // Ensure that the alignment is a power of two
  alignment = static_cast<std::size_t>(sln::next_power_of_two(std::max(alignment, std::size_t{2})));
  // Ensure that the nr of bytes reserved is a multiple of the alignment
  nr_bytes = (nr_bytes % alignment == 0) ? nr_bytes : (nr_bytes + alignment - (nr_bytes % alignment));

  auto ptr = static_cast<std::uint8_t*>(std::aligned_alloc(alignment, nr_bytes));
  return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(ptr, nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `std::free`.
 *
 * \param data A pointer to previously allocated data.
 */
void AlignedMallocAllocator::deallocate(std::uint8_t*& data) noexcept
{
  std::free(data);
  data = nullptr;
}


/** \brief Allocates the specified number of bytes via (non-throwing) `new` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<NewAllocator> NewAllocator::allocate(std::size_t nr_bytes) noexcept
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<NewAllocator>(nullptr, 0);
  }

  const auto ptr = ::new (std::nothrow) std::uint8_t[nr_bytes];
  return construct_memory_block_from_existing_memory<NewAllocator>(ptr, (ptr == nullptr) ? 0 : nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `delete`.
 *
 * \param data A pointer to previously allocated data.
 */
void NewAllocator::deallocate(std::uint8_t*& data) noexcept
{
  ::delete[] data;
  data = nullptr;
}


/** \brief Allocates the specified number of bytes via (non-throwing) `new` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \param alignment The required byte alignment. Needs to be a power of two (e.g. 8, 16, 32, ...).
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<AlignedNewAllocator> AlignedNewAllocator::allocate(std::size_t nr_bytes, std::size_t alignment) noexcept
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<AlignedNewAllocator>(nullptr, 0);
  }

  // Ensure that the alignment is a power of two
  alignment = static_cast<std::size_t>(sln::next_power_of_two(std::max(alignment, std::size_t{2})));

  const auto ptr = ::new (std::align_val_t{alignment}, std::nothrow) std::uint8_t[nr_bytes];
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `delete`.
 *
 * \param data A pointer to previously allocated data.
 */
void AlignedNewAllocator::deallocate(std::uint8_t*& data) noexcept
{
  ::delete[] data;
  data = nullptr;
}

}  // namespace sln

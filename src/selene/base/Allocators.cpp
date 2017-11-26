// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/Allocators.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <new>

namespace selene {

/** \brief Allocates the specified number of bytes via `std::malloc` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<MallocAllocator> MallocAllocator::allocate(std::size_t nr_bytes)
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
void MallocAllocator::deallocate(std::uint8_t*& data)
{
  if (data != nullptr)
  {
    std::free(data);
  }

  data = nullptr;
}


/** \brief Allocates the specified number of bytes via (non-throwing) `new` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<NewAllocator> NewAllocator::allocate(std::size_t nr_bytes)
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<NewAllocator>(nullptr, 0);
  }

  auto ptr = new(std::nothrow) std::uint8_t[nr_bytes];
  return construct_memory_block_from_existing_memory<NewAllocator>(ptr, (ptr == nullptr) ? 0 : nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `delete`.
 *
 * \param data A pointer to previously allocated data.
 */
void NewAllocator::deallocate(std::uint8_t*& data)
{
  delete[] data;
  data = nullptr;
}

} // namespace selene
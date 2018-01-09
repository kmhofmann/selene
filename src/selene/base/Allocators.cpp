// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
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
  if (data != nullptr)
  {
    std::free(data);
  }

  data = nullptr;
}


/** \brief Allocates the specified number of bytes via `std::malloc` and returns a MemoryBlock.
 *
 * \param nr_bytes The number of bytes to allocate.
 * \param alignment The required byte alignment. Needs to be a power of two (e.g. 8, 16, 32, ...).
 * \return A MemoryBlock instance with a pointer to the allocated data. If no data could be allocated, the MemoryBlock
 *         will point to nullptr.
 */
MemoryBlock<AlignedMallocAllocator> AlignedMallocAllocator::allocate(std::size_t nr_bytes, std::size_t alignment) noexcept
{
  if (nr_bytes == 0)
  {
    return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(nullptr, 0);
  }

  // Ensure that the alignment is a power of two
  alignment = sln::next_power_of_two(std::max(alignment, std::size_t{2}));

  // TODO: C++17's aligned_alloc (http://en.cppreference.com/w/cpp/memory/c/aligned_alloc) would make life easier...

  // Allocate extra space for storing the alignment offset, and to fit the alignment itself
  const auto offset_ptr_storage = std::size_t{sizeof(void*)};
  const auto offset_alignment = alignment;
  void* const m_ptr = std::malloc(offset_ptr_storage + offset_alignment + nr_bytes);

  if (m_ptr == nullptr)
  {
    return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(nullptr, 0);
  }

  // Compute the aligned pointer
  auto m_ptr_2 = reinterpret_cast<void*>(reinterpret_cast<std::uint8_t*>(m_ptr) + offset_ptr_storage);
  std::size_t space = offset_alignment + nr_bytes;
  void* const a_ptr = std::align(alignment, sizeof(std::uint8_t), m_ptr_2, space);

  if (a_ptr == nullptr)
  {
    return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(nullptr, 0);
  }

  // Store malloc'ed pointer before the aligned memory block
  (reinterpret_cast<void**>(a_ptr))[-1] = m_ptr;

  auto ptr = reinterpret_cast<std::uint8_t*>(a_ptr);
  return construct_memory_block_from_existing_memory<AlignedMallocAllocator>(ptr, nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `std::free`.
 *
 * \param data A pointer to previously allocated data.
 */
void AlignedMallocAllocator::deallocate(std::uint8_t*& data) noexcept
{
  if (data != nullptr)
  {
    // Retrieve originally malloc'ed pointer from stored position
    std::uint8_t* data_ptr = data;
    void* m_ptr = (reinterpret_cast<void**>(data_ptr))[-1];
    std::free(m_ptr);
  }

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
  alignment = sln::next_power_of_two(std::max(alignment, std::size_t{2}));

  // Allocate extra space for storing the alignment offset, and to fit the alignment itself
  const auto offset_ptr_storage = std::size_t{sizeof(void*)};
  const auto offset_alignment = alignment;
  const auto n_ptr = ::new (std::nothrow) std::uint8_t[offset_ptr_storage + offset_alignment + nr_bytes];

  if (n_ptr == nullptr)
  {
    return construct_memory_block_from_existing_memory<AlignedNewAllocator>(nullptr, 0);
  }

  // Compute the aligned pointer
  auto m_ptr_2 = static_cast<void*>(n_ptr + offset_ptr_storage);
  std::size_t space = offset_alignment + nr_bytes;
  void* const a_ptr = std::align(alignment, sizeof(std::uint8_t), m_ptr_2, space);

  if (a_ptr == nullptr)
  {
    return construct_memory_block_from_existing_memory<AlignedNewAllocator>(nullptr, 0);
  }

  // Store new'ed pointer before the aligned memory block
  (reinterpret_cast<std::uint8_t**>(a_ptr))[-1] = n_ptr;

  auto ptr = reinterpret_cast<std::uint8_t*>(a_ptr);
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, nr_bytes);
}

/** \brief Deallocates the previously allocated memory block using `delete`.
 *
 * \param data A pointer to previously allocated data.
 */
void AlignedNewAllocator::deallocate(std::uint8_t*& data) noexcept
{
  if (data != nullptr)
  {
    // Retrieve originally new'ed pointer from stored position
    std::uint8_t* data_ptr = data;
    std::uint8_t* n_ptr = (reinterpret_cast<std::uint8_t**>(data_ptr))[-1];
    delete[] n_ptr;
  }

  data = nullptr;
}

}  // namespace sln

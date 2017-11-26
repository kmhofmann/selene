// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_ALLOCATORS_HPP
#define SELENE_BASE_ALLOCATORS_HPP

#include <cstdint>
#include <cstdlib>

namespace selene {

template <typename Allocator> class MemoryBlock;

/** \brief Provides means for memory allocation and deallocation throughout the library.
 *
 * The MallocAllocator just wraps `std::malloc` and `std::free` in a consistent interface.
 *
 *  Used in various places inside the library. Not recommended for memory management outside of the library.
 */
struct MallocAllocator
{
  static MemoryBlock<MallocAllocator> allocate(std::size_t nr_bytes);
  static void deallocate(std::uint8_t*& data);
};

/** \brief Provides means for memory allocation and deallocation throughout the library.
 *
 * The NewAllocator just wraps `new` and `delete` in a consistent interface.
 *
 *  Used in various places inside the library. Not recommended for memory management outside of the library.
 */
struct NewAllocator
{
  static MemoryBlock<NewAllocator> allocate(std::size_t nr_bytes);
  static void deallocate(std::uint8_t*& data);
};

} // namespace selene

#endif // SELENE_BASE_ALLOCATORS_HPP

// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_MEMORY_VIEW_HPP
#define SELENE_BASE_MEMORY_VIEW_HPP

#include <cstdint>

namespace selene {

/** Represents a non-owned, read-only block of memory.
 */
struct MemoryView
{
  std::uint8_t* const data; ///< Non-owning pointer to memory.
  const std::size_t size; ///< Size of the non-owned memory block.

  /** Constructor, taking a pointer to memory and a size.
   *
   * \param data_ Pointer to memory.
   * \param size_ Corresponding size.
   */
  MemoryView(std::uint8_t* const data_, std::size_t size_)
      : data(data_), size(size_)
  { }
};

} // namespace selene

#endif // SELENE_BASE_MEMORY_VIEW_HPP
// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/base/Allocators.hpp>
#include <selene/base/MemoryBlock.hpp>
#include <selene/base/Utils.hpp>

#include <random>

TEST_CASE("Allocators", "[base]")
{
  constexpr std::size_t N = 1000;  // number of allocation repeats for each allocator

  std::mt19937 rng(500);
  std::uniform_int_distribution<std::size_t> dist_bytes(1, 1024 * 1024);
  std::uniform_int_distribution<std::size_t> dist_alignment(3, 8);

  SECTION("Malloc allocator")
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      const auto nr_bytes_to_allocate = dist_bytes(rng);
      auto memory_block = sln::MallocAllocator::allocate(nr_bytes_to_allocate);
      REQUIRE(memory_block.size() == nr_bytes_to_allocate);
    }
  }

  SECTION("Aligned malloc allocator")
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      const auto nr_bytes_to_allocate = dist_bytes(rng);
      const auto alignment = sln::power(std::size_t{2}, dist_alignment(rng));
      auto memory_block = sln::AlignedMallocAllocator::allocate(nr_bytes_to_allocate, alignment);
      REQUIRE(memory_block.size() >= nr_bytes_to_allocate);
      REQUIRE(reinterpret_cast<std::uintptr_t>(memory_block.data()) % alignment == 0);
    }
  }

  SECTION("New allocator")
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      const auto nr_bytes_to_allocate = dist_bytes(rng);
      auto memory_block = sln::NewAllocator::allocate(nr_bytes_to_allocate);
      REQUIRE(memory_block.size() == nr_bytes_to_allocate);
    }
  }

  SECTION("Aligned new allocator")
  {
    for (std::size_t i = 0; i < N; ++i)
    {
      const auto nr_bytes_to_allocate = dist_bytes(rng);
      const auto alignment = sln::power(std::size_t{2}, dist_alignment(rng));
      auto memory_block = sln::AlignedNewAllocator::allocate(nr_bytes_to_allocate, alignment);
      REQUIRE(memory_block.size() >= nr_bytes_to_allocate);
      REQUIRE(reinterpret_cast<std::uintptr_t>(memory_block.data()) % alignment == 0);
    }
  }
}
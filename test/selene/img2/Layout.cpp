// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img2/Layout.hpp>

using namespace sln::literals;

TEST_CASE("Layout construction", "[img]")
{
  sln::TypedLayout l0{10_px, 10_px};
  sln::TypedLayout l1{10_px, 10_px, sln::Stride{40}};
  const auto l2 = l0;
  CHECK(l0 == l2);

}



// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_TYPES_HPP
#define SELENE_BASE_TYPES_HPP

namespace selene {

using float32_t = float;
using float64_t = double;

static_assert(sizeof(float32_t) == 4, "type size mismatch");
static_assert(sizeof(float64_t) == 8, "type size mismatch");

} // namespace selene

#endif // SELENE_BASE_TYPES_HPP
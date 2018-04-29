// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/PixelConversions.hpp>

namespace sln {

constexpr std::array<default_float_t, 3> detail::RGBToYCoefficients::values;
constexpr std::array<default_float_t, 3> detail::BGRToYCoefficients::values;

}  // namespace sln

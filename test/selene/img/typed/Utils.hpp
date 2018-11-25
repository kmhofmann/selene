// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_TEST_IMG_TYPED_UTILS_HPP
#define SELENE_TEST_IMG_TYPED_UTILS_HPP

#include <selene/img/typed/Image.hpp>

#include <test/selene/Utils.hpp>

#include <random>

namespace sln_test {

template <typename PixelType, typename RNG>
sln::Image<PixelType> construct_random_image(sln::PixelLength width, sln::PixelLength height, RNG& rng)
{
  using namespace sln::literals;

  static_assert(sln::PixelTraits<PixelType>::nr_channels == 1, "Unexpected multi-channel image");

  using Element = typename sln::PixelTraits<PixelType>::Element;

  constexpr auto is_int = sln::PixelTraits<PixelType>::is_integral;
  auto die = sln_test::uniform_distribution<Element>(Element{0},
                                                     is_int ? std::numeric_limits<Element>::max() : Element{1});

  std::uniform_int_distribution<std::uint16_t> die_stride(0, 16);
  const auto extra_stride_bytes = std::size_t{die_stride(rng) * sizeof(Element)};
  const auto stride_bytes = sln::Stride(width * sln::PixelTraits<PixelType>::nr_bytes + extra_stride_bytes);
  sln::Image<PixelType> img({width, height, stride_bytes});

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      *ptr++ = PixelType{die(rng)};
    }
  }

  return img;
}

}  // namespace sln_test

#endif  // SELENE_TEST_IMG_TYPED_UTILS_HPP

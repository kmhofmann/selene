// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_TEST_IMG_DYNAMIC_UTILS_HPP
#define SELENE_TEST_IMG_DYNAMIC_UTILS_HPP

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <test/selene/Utils.hpp>

#include <random>

namespace sln_test {

template <typename PixelType, typename RNG>
sln::DynImage construct_random_dynamic_image(sln::PixelLength width, sln::PixelLength height, RNG& rng)
{
  using namespace sln::literals;
  using Element = typename sln::PixelTraits<PixelType>::Element;

  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = sln::PixelTraits<PixelType>::nr_bytes_per_channel;

  constexpr auto is_int = sln::PixelTraits<PixelType>::is_integral;
  auto die = sln_test::uniform_distribution<Element>(Element{0},
                                                     is_int ? std::numeric_limits<Element>::max() : Element{1});

  std::uniform_int_distribution<std::uint16_t> die_stride(0, 16);
  const auto extra_stride_bytes = std::ptrdiff_t(die_stride(rng) * sizeof(Element));
  const auto stride_bytes = sln::Stride(width * sln::PixelTraits<PixelType>::nr_bytes + extra_stride_bytes);
  const auto semantics = sln::UntypedImageSemantics{sln::PixelTraits<PixelType>::pixel_format,
                                                    sln::PixelTraits<PixelType>::sample_format};
  sln::DynImage img{{width, height, nr_channels, nr_bytes_per_channel, stride_bytes}, semantics};

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    auto ptr = img.data<PixelType>(y);
    for (auto x = 0_idx; x < img.width(); ++x, ++ptr)
    {
      for (auto c = std::int16_t{0}; c < nr_channels; ++c)
      {
        (*ptr)[c] = static_cast<Element>(die(rng));
      }
    }
  }

  return img;
}

}  // namespace sln_test

#endif  // SELENE_TEST_IMG_DYNAMIC_UTILS_HPP

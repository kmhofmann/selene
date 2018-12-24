// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_TEST_IMG_TYPED_UTILS_HPP
#define SELENE_TEST_IMG_TYPED_UTILS_HPP

#include <selene/img/typed/Image.hpp>

#include <test/selene/Utils.hpp>

#include <random>

namespace sln_test {

inline sln::Image<sln::Pixel<std::uint8_t, 1>> make_3x3_test_image_8u1()
{
  using namespace sln::literals;
  sln::Image<sln::Pixel<std::uint8_t, 1>> img({3_px, 3_px});

  img(0_idx, 0_idx) = 10;
  img(1_idx, 0_idx) = 20;
  img(2_idx, 0_idx) = 30;

  img(0_idx, 1_idx) = 40;
  img(1_idx, 1_idx) = 50;
  img(2_idx, 1_idx) = 60;

  img(0_idx, 2_idx) = 70;
  img(1_idx, 2_idx) = 80;
  img(2_idx, 2_idx) = 90;

  return img;
}

inline sln::Image<sln::Pixel<std::uint8_t, 3>> make_3x3_test_image_8u3()
{
  using namespace sln::literals;
  sln::Image<sln::Pixel<std::uint8_t, 3>> img({3_px, 3_px});

  img(0_idx, 0_idx) = sln::Pixel<std::uint8_t, 3>(10, 11, 12);
  img(1_idx, 0_idx) = sln::Pixel<std::uint8_t, 3>(20, 21, 22);
  img(2_idx, 0_idx) = sln::Pixel<std::uint8_t, 3>(30, 31, 32);

  img(0_idx, 1_idx) = sln::Pixel<std::uint8_t, 3>(40, 41, 42);
  img(1_idx, 1_idx) = sln::Pixel<std::uint8_t, 3>(50, 51, 52);
  img(2_idx, 1_idx) = sln::Pixel<std::uint8_t, 3>(60, 61, 62);

  img(0_idx, 2_idx) = sln::Pixel<std::uint8_t, 3>(70, 71, 72);
  img(1_idx, 2_idx) = sln::Pixel<std::uint8_t, 3>(80, 81, 82);
  img(2_idx, 2_idx) = sln::Pixel<std::uint8_t, 3>(90, 91, 92);

  return img;
}

template <typename PixelType, typename RNG, typename Distribution>
PixelType construct_random_pixel(RNG& rng, Distribution& dist)
{
  using ElementType = typename sln::PixelTraits<PixelType>::Element;
  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;

  PixelType px;
  for (std::size_t i = 0; i < nr_channels; ++i)
  {
    px[i] = static_cast<ElementType>(dist(rng));
  }
  return px;
}

template <typename PixelType, typename RNG>
sln::Image<PixelType> construct_random_image(sln::PixelLength width, sln::PixelLength height, RNG& rng)
{
  using namespace sln::literals;
  using Element = typename sln::PixelTraits<PixelType>::Element;

  constexpr auto is_int = sln::PixelTraits<PixelType>::is_integral;
  auto dist = sln_test::uniform_distribution<Element>(Element{0},
                                                      is_int ? std::numeric_limits<Element>::max() : Element{1});

  std::uniform_int_distribution<std::uint16_t> die_stride(0, 16);
  const auto extra_stride_bytes = std::ptrdiff_t(die_stride(rng) * sizeof(Element));
  const auto stride_bytes = sln::Stride(width * sln::PixelTraits<PixelType>::nr_bytes + extra_stride_bytes);
  sln::Image<PixelType> img({width, height, stride_bytes});

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      *ptr++ = construct_random_pixel<PixelType>(rng, dist);
    }
  }

  return img;
}

}  // namespace sln_test

#endif  // SELENE_TEST_IMG_TYPED_UTILS_HPP

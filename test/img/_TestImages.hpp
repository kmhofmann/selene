// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_TEST_IMG_TEST_IMAGES_HPP
#define SELENE_TEST_IMG_TEST_IMAGES_HPP

#include <selene/img/Image.hpp>
#include <selene/img/PixelTraits.hpp>
#include <selene/img/Types.hpp>
#include <selene/img_ops/Algorithms.hpp>

#include <limits>

#include <test/Utils.hpp>

namespace sln_test {

sln::Image_8u1 make_3x3_test_image_8u1();

sln::Image_8u3 make_3x3_test_image_8u3();

template <typename PixelType, typename Distribution>
PixelType make_random_pixel(std::mt19937& rng, Distribution& dist)
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

template <typename PixelType>
sln::Image<PixelType> make_random_image(sln::PixelIndex width, sln::PixelIndex height, std::mt19937& rng)
{
  using Element = typename sln::PixelTraits<PixelType>::Element;

  auto dist = uniform_distribution(std::numeric_limits<Element>::min(), std::numeric_limits<Element>::max());
  sln::Image<PixelType> img(width, height);
  sln::for_each_pixel(img, [&rng, &dist](auto& px){ px = make_random_pixel<PixelType>(rng, dist); });
  return img;
}

}  // namespace sln_test

#endif  // SELENE_TEST_IMG_TEST_IMAGES_HPP

// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <test/selene/img/dynamic/Utils.hpp>

using namespace sln::literals;

namespace {

template <typename PixelType>
void compare_iteration(sln::DynImage& img)
{
  // const iteration

  {
    const auto& img2 = img;

    std::vector<PixelType> elements_0;
    for (auto y = 0_idx; y < img2.height(); ++y)
    {
      for (auto x = 0_idx; x < img2.width(); ++x)
      {
        elements_0.emplace_back(img2.pixel<PixelType>(x, y));
      }
    }

    std::vector<PixelType> elements_1;
    for (sln::DynImage::const_iterator<PixelType> it = img2.cbegin<PixelType>(); it != img2.cend<PixelType>(); ++it)
    {
      const auto& row = *it;
      for (auto it_el = row.cbegin(); it_el != row.cend(); ++it_el)
      {
        const PixelType& el = *it_el;
        elements_1.emplace_back(el);
      }
    }

    REQUIRE(elements_0 == elements_1);
  }

  // non-const iteration

  {
    std::vector<PixelType> elements_0;
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        elements_0.emplace_back(img.pixel<PixelType>(x, y));
      }
    }

    std::vector<PixelType> elements_1;
    for (sln::DynImage::iterator<PixelType> it = img.begin<PixelType>(); it != img.end<PixelType>(); ++it)
    {
      auto& row = *it;
      for (auto it_el = row.begin(); it_el != row.end(); ++it_el)
      {
        PixelType& el = *it_el;
        elements_1.emplace_back(el);
        el = PixelType{17};  // overwrite image element
      }
    }

    // check that elements have all been overwritten
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img.pixel<PixelType>(x, y) == PixelType{17});
      }
    }

    REQUIRE(elements_0 == elements_1);
  }
}

template <typename ElementType, typename RNG>
void random_iteration(sln::PixelLength w, sln::PixelLength h, RNG& rng)
{
  using PixelType = sln::Pixel<ElementType, 1>;
  auto img = sln_test::construct_random_dynamic_image<PixelType>(w, h, rng);
  compare_iteration<PixelType>(img);
}

}  // namespace


TEST_CASE("Dynamic Image iteration", "[img]")
{
  std::mt19937 rng(42ul);

  for (auto h = 1_px; h < 16_px; ++h)
  {
    for (auto w = 1_px; w < 16_px; ++w)
    {
      random_iteration<std::uint8_t>(w, h, rng);
      random_iteration<std::int8_t>(w, h, rng);
      random_iteration<std::uint16_t>(w, h, rng);
      random_iteration<std::int16_t>(w, h, rng);
      random_iteration<std::uint32_t>(w, h, rng);
      random_iteration<std::int32_t>(w, h, rng);
      random_iteration<std::uint64_t>(w, h, rng);
      random_iteration<std::int64_t>(w, h, rng);
      random_iteration<sln::float32_t>(w, h, rng);
      random_iteration<sln::float64_t>(w, h, rng);
    }
  }
}

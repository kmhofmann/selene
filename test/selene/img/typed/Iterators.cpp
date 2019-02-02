// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/Image.hpp>

#include <test/selene/img/typed/_Utils.hpp>

using namespace sln::literals;

namespace {

template <typename T>
void compare_iteration(sln::Image<T>& img)
{
  // const iteration

  {
    const auto& img2 = img;

    std::vector<T> elements_0;
    for (auto y = 0_idx; y < img2.height(); ++y)
    {
      for (auto x = 0_idx; x < img2.width(); ++x)
      {
        elements_0.emplace_back(img2(x, y));
      }
    }

    sln::PixelIndex y_count{0};
    std::vector<T> elements_1;
    for (const auto& row : img2)
    {
      REQUIRE(row.index() == y_count);
      for (const auto& el : row)
      {
        elements_1.emplace_back(el);
      }
      ++y_count;
    }

    std::vector<T> elements_2;
    for (typename sln::Image<T>::const_iterator it = img2.cbegin(); it != img2.cend(); ++it)
    {
      const auto& row = *it;
      for (auto it_el = row.cbegin(); it_el != row.cend(); ++it_el)
      {
        const T& el = *it_el;
        elements_2.emplace_back(el);
      }
    }

    REQUIRE(elements_0 == elements_1);
    REQUIRE(elements_0 == elements_2);
  }

  // non-const iteration

  {
    std::vector<T> elements_0;
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        elements_0.emplace_back(img(x, y));
      }
    }

    sln::PixelIndex y_count{0};
    std::vector<T> elements_1;
    for (auto& row : img)
    {
      REQUIRE(row.index() == y_count);
      for (auto& el : row)
      {
        elements_1.emplace_back(el);
      }
      ++y_count;
    }

    std::vector<T> elements_2;
    for (typename sln::Image<T>::iterator it = img.begin(); it != img.end(); ++it)
    {
      auto& row = *it;
      for (auto it_el = row.begin(); it_el != row.end(); ++it_el)
      {
        T& el = *it_el;
        elements_2.emplace_back(el);
        el = T{17};  // overwrite image element
      }
    }

    // check that elements have all been overwritten
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == T{17});
      }
    }

    REQUIRE(elements_0 == elements_1);
    REQUIRE(elements_0 == elements_2);
  }

  auto it_begin = img.begin();
  auto it_cbegin = img.cbegin();
  auto it_end = img.end();
  auto it_cend = img.cend();
  REQUIRE(it_begin == it_cbegin);
  REQUIRE(it_end == it_cend);
  REQUIRE(it_begin != it_end);
  REQUIRE(it_cbegin != it_cend);
}

template <typename ElementType, typename RNG>
void random_iteration(sln::PixelLength w, sln::PixelLength h, RNG& rng)
{
  auto img = sln_test::construct_random_image<sln::Pixel<ElementType, 1>>(w, h, rng);
  compare_iteration(img);
}

}  // namespace


TEST_CASE("Image iteration", "[img]")
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

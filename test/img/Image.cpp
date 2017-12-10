// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>
#include <selene/img/Image.hpp>

#include <random>

using namespace selene;
using namespace selene::img;

namespace {

template <typename T>
void basic_image_tests(Length width, Length height, T fill_value)
{
  Image<T> img(width, height);
  REQUIRE((img.width() == width));
  REQUIRE((img.height() == height));
  REQUIRE(img.stride_bytes() == img.width() * sizeof(T));
  REQUIRE(img.is_packed());
  REQUIRE(!img.is_view());
  REQUIRE(!img.is_empty());

  img.fill(fill_value);
  for (Index y = 0_px; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (Index x = 0_px; x < img.width(); ++x)
    {
      REQUIRE(img(x, y) == fill_value);
      REQUIRE(*(ptr + x) == fill_value);
    }
  }

  Image<T> img2;
  clone(img, img2);
  REQUIRE(img2.width() == img.width());
  REQUIRE(img2.height() == img.height());
  REQUIRE(img2.stride_bytes() == img.stride_bytes());
}

template <typename T, typename RNG>
Image<T> construct_random_image(Length width, Length height, RNG& rng)
{
  static_assert(PixelTraits<T>::nr_channels == 1);

  using Element = typename PixelTraits<T>::Element;

  constexpr auto is_int = PixelTraits<T>::is_integral;
  constexpr auto is_fp = PixelTraits<T>::is_floating_point;
  using DieType = std::conditional_t<is_int, std::uniform_int_distribution<Element>,
      std::conditional_t<is_fp, std::uniform_real_distribution<Element>, void>>;
  DieType die;

  std::uniform_int_distribution<std::uint8_t> die_stride(0, 16);
  const auto extra_stride_bytes = std::size_t{die_stride(rng)};
  const auto stride_bytes = Bytes(width * PixelTraits<T>::nr_bytes + extra_stride_bytes);
  Image<T> img(width, height, stride_bytes);

  for (Index y = 0_px; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (Index x = 0_px; x < img.width(); ++x)
    {
      *ptr++ = T(die(rng));
    }
  }

  return img;
}

template <typename T>
void compare_iteration(Image<T>& img)
{
  // const iteration

  {
    const auto& img2 = img;

    std::vector<T> elements_0;
    for (Index y = 0_px; y < img2.height(); ++y)
    {
      for (Index x = 0_px; x < img2.width(); ++x)
      {
        elements_0.emplace_back(img2(x, y));
      }
    }

    std::uint64_t y_count{0};
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
    for (typename Image<T>::const_iterator it = img2.cbegin(); it != img2.cend(); ++it)
    {
      const ConstImageRow<T>& row = *it;
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
    for (Index y = 0_px; y < img.height(); ++y)
    {
      for (Index x = 0_px; x < img.width(); ++x)
      {
        elements_0.emplace_back(img(x, y));
      }
    }

    std::uint64_t y_count{0};
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
    for (typename Image<T>::iterator it = img.begin(); it != img.end(); ++it)
    {
      ImageRow<T>& row = *it;
      for (auto it_el = row.begin(); it_el != row.end(); ++it_el)
      {
        T& el = *it_el;
        elements_2.emplace_back(el);
        el = T{17}; // overwrite image element
      }
    }

    // check that elements have all been overwritten
    for (Index y = 0_px; y < img.height(); ++y)
    {
      for (Index x = 0_px; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == T{17});
      }
    }

    REQUIRE(elements_0 == elements_1);
    REQUIRE(elements_0 == elements_2);
  }
}

template <typename ElementType, typename RNG>
void random_iteration(Index w, Index h, RNG& rng)
{
  auto img = construct_random_image<Pixel<ElementType, 1>>(w, h, rng);
  compare_iteration(img);
}

}  // namespace _


TEST_CASE("Image construction", "[img]")
{
  for (Index h = 10_px; h < 100_px; h += 20_px)
  {
    for (Index w = 10_px; w < 100_px; w += 10_px)
    {
      basic_image_tests<unsigned char>(w, h, 42);
    }
  }
}

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
      random_iteration<float32_t>(w, h, rng);
      random_iteration<float64_t>(w, h, rng);
    }
  }
}